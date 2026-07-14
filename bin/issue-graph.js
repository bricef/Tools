#!/usr/bin/env node
// issue-graph — render a GitHub repo's issue hierarchy (sub-issues) as a
// self-contained interactive HTML page. Uses `gh` for auth; no npm deps.

import { execFileSync } from 'node:child_process';
import { readFileSync, writeFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));

const USAGE = `usage: issue-graph [owner/repo] [options]

Fetches all issues of a GitHub repo (open and closed) with their sub-issue
parent/child links and writes a self-contained HTML graph.

With no repo argument, uses the GitHub repo of the current directory.

options:
  -o, --out FILE     output path (default: <repo>-issues.html)
      --json FILE    also dump the fetched issue data as JSON
      --from-json F  render from a previously dumped JSON file (no fetching)
  -h, --help         show this help

requires: gh (authenticated), unless --from-json is used`;

function die(msg) {
  process.stderr.write(`issue-graph: ${msg}\n`);
  process.exit(1);
}

let repo = null, out = null, jsonOut = null, fromJson = null;
{
  const args = process.argv.slice(2);
  for (let i = 0; i < args.length; i++) {
    const a = args[i];
    if (a === '-h' || a === '--help') { console.log(USAGE); process.exit(0); }
    else if (a === '-o' || a === '--out') out = args[++i];
    else if (a === '--json') jsonOut = args[++i];
    else if (a === '--from-json') fromJson = args[++i];
    else if (!a.startsWith('-') && !repo) repo = a;
    else die(`unexpected argument: ${a}\n${USAGE}`);
  }
}

function gh(args) {
  try {
    return execFileSync('gh', args, { encoding: 'utf8', maxBuffer: 256 * 1024 * 1024 });
  } catch (e) {
    const detail = (e.stderr || e.message || '').toString().trim();
    die(`gh ${args[0]} failed${detail ? `:\n${detail}` : ''}`);
  }
}

const QUERY = `query($owner: String!, $name: String!, $cursor: String) {
  repository(owner: $owner, name: $name) {
    issues(first: 100, after: $cursor, states: [OPEN, CLOSED],
           orderBy: {field: CREATED_AT, direction: ASC}) {
      pageInfo { hasNextPage endCursor }
      nodes {
        number title state stateReason url createdAt closedAt
        author { login }
        issueType { name color }
        milestone { title }
        labels(first: 20) { nodes { name color } }
        assignees(first: 10) { nodes { login } }
        parent { number repository { nameWithOwner } }
        subIssuesSummary { total completed }
      }
    }
  }
}`;

let payload;
if (fromJson) {
  payload = JSON.parse(readFileSync(fromJson, 'utf8'));
  repo = payload.repo;
} else {
  if (!repo) {
    try {
      repo = execFileSync('gh', ['repo', 'view', '--json', 'nameWithOwner', '--jq', '.nameWithOwner'],
        { encoding: 'utf8' }).trim();
    } catch {
      die(`no repo given and the current directory is not a GitHub repo\n${USAGE}`);
    }
  }
  if (!/^[\w.-]+\/[\w.-]+$/.test(repo)) die(`invalid repo (expected owner/name): ${repo}`);
  const [owner, name] = repo.split('/');

  const raw = [];
  let cursor = null;
  for (;;) {
    const args = ['api', 'graphql', '-f', `query=${QUERY}`, '-F', `owner=${owner}`, '-F', `name=${name}`];
    if (cursor) args.push('-F', `cursor=${cursor}`);
    const res = JSON.parse(gh(args));
    const conn = res.data?.repository?.issues;
    if (!conn) die(`no issue data returned for ${repo} — does the repo exist and can you see it?`);
    raw.push(...conn.nodes);
    process.stderr.write(`\rfetched ${raw.length} issues`);
    if (!conn.pageInfo.hasNextPage) break;
    cursor = conn.pageInfo.endCursor;
  }
  process.stderr.write('\n');

  const issues = raw.map(n => ({
    number: n.number,
    title: n.title,
    state: n.state,
    stateReason: n.stateReason,
    url: n.url,
    createdAt: n.createdAt,
    closedAt: n.closedAt,
    author: n.author?.login ?? null,
    type: n.issueType ? { name: n.issueType.name, color: n.issueType.color } : null,
    milestone: n.milestone?.title ?? null,
    labels: n.labels.nodes.map(l => ({ name: l.name, color: l.color })),
    assignees: n.assignees.nodes.map(a => a.login),
    parent: n.parent ? { number: n.parent.number, repo: n.parent.repository.nameWithOwner } : null,
    sub: { total: n.subIssuesSummary?.total ?? 0, done: n.subIssuesSummary?.completed ?? 0 },
  }));

  payload = { repo, generatedAt: new Date().toISOString(), issues };
}

if (jsonOut) writeFileSync(jsonOut, JSON.stringify(payload, null, 2));

const template = readFileSync(join(HERE, '..', 'template.html'), 'utf8');
// () => replacements: the payload may contain `$&`-style sequences that
// String.replace would otherwise expand.
const json = JSON.stringify(payload).replace(/</g, '\\u003c');
const html = template
  .replace(/__TITLE__/g, () => repo.replace(/&/g, '&amp;').replace(/</g, '&lt;'))
  .replace('/*__DATA__*/null', () => json);
if (html === template) die('template placeholder not found — template.html is corrupted');

const outPath = resolve(out ?? `${repo.split('/')[1]}-issues.html`);
writeFileSync(outPath, html);

const linked = payload.issues.filter(i => i.parent || i.sub.total > 0).length;
console.log(`${payload.issues.length} issues (${linked} in hierarchies) → ${outPath}`);
