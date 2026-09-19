#!/usr/bin/env node
// issue-graph — render a GitHub repo's issue hierarchy (sub-issues) as a
// self-contained interactive HTML page, or serve it live. Uses `gh` for
// auth; no npm deps.

import { execFile, execFileSync } from 'node:child_process';
import { readFileSync, writeFileSync } from 'node:fs';
import { createServer } from 'node:http';
import { createHash } from 'node:crypto';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { promisify } from 'node:util';

const HERE = dirname(fileURLToPath(import.meta.url));
const execFileP = promisify(execFile);

const USAGE = `usage: issue-graph [owner/repo] [options]          render a static HTML file
       issue-graph serve [owner/repo] [options]    live-updating local server

Fetches all issues of a GitHub repo (open and closed) with their sub-issue
parent/child links, rendered as an interactive HTML graph.

With no repo argument, uses the GitHub repo of the current directory.

static options:
  -o, --out FILE     output path (default: <repo>-issues.html)
      --json FILE    also dump the fetched issue data as JSON
      --from-json F  render from a previously dumped JSON file (no fetching)

serve options:
  -p, --port N       port to listen on (default: 8422)
      --host ADDR    bind address (default: 127.0.0.1 — the server exposes
                     whatever your gh login can see, so think before widening)
  -i, --interval N   refresh interval in seconds; client poll rate and server
                     cache TTL (default: 30, min: 1)
      --from-json F  serve a JSON dump, re-read on every refresh (no gh);
                     useful for testing and non-GitHub data sources

  -h, --help         show this help

requires: gh (authenticated), unless --from-json is used`;

function die(msg) {
  process.stderr.write(`issue-graph: ${msg}\n`);
  process.exit(1);
}

const REPO_RE = /^[\w.-]+\/[\w.-]+$/;

function repoFromCwd() {
  try {
    return execFileSync('gh', ['repo', 'view', '--json', 'nameWithOwner', '--jq', '.nameWithOwner'],
      { encoding: 'utf8' }).trim();
  } catch {
    return null;
  }
}

async function gh(args) {
  try {
    const { stdout } = await execFileP('gh', args, { encoding: 'utf8', maxBuffer: 256 * 1024 * 1024 });
    return stdout;
  } catch (e) {
    throw new Error((e.stderr || e.message || 'gh failed').toString().trim());
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

async function fetchPayload(repo, { quiet = false } = {}) {
  const [owner, name] = repo.split('/');
  const raw = [];
  let cursor = null;
  for (;;) {
    const args = ['api', 'graphql', '-f', `query=${QUERY}`, '-F', `owner=${owner}`, '-F', `name=${name}`];
    if (cursor) args.push('-F', `cursor=${cursor}`);
    const res = JSON.parse(await gh(args));
    const conn = res.data?.repository?.issues;
    if (!conn) throw new Error(`no issue data returned for ${repo} — does the repo exist and can you see it?`);
    raw.push(...conn.nodes);
    if (!quiet) process.stderr.write(`\rfetched ${raw.length} issues`);
    if (!conn.pageInfo.hasNextPage) break;
    cursor = conn.pageInfo.endCursor;
  }
  if (!quiet) process.stderr.write('\n');

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

  return { repo, generatedAt: new Date().toISOString(), issues };
}

const TEMPLATE = readFileSync(join(HERE, '..', 'template.html'), 'utf8');
if (!TEMPLATE.includes('/*__DATA__*/null') || !TEMPLATE.includes('/*__LIVE__*/null'))
  die('template placeholders missing — template.html is corrupted');

// () => replacements: payloads may contain `$&`-style sequences that
// String.replace would otherwise expand.
function renderPage({ title, data = null, live = null }) {
  let html = TEMPLATE.replace(/__TITLE__/g,
    () => title.replace(/&/g, '&amp;').replace(/</g, '&lt;'));
  if (data) html = html.replace('/*__DATA__*/null', () => JSON.stringify(data).replace(/</g, '\\u003c'));
  if (live) html = html.replace('/*__LIVE__*/null', () => JSON.stringify(live).replace(/</g, '\\u003c'));
  return html;
}

/* ---------------- static mode ---------------- */

async function staticMain(args) {
  let repo = null, out = null, jsonOut = null, fromJson = null;
  for (let i = 0; i < args.length; i++) {
    const a = args[i];
    if (a === '-h' || a === '--help') { console.log(USAGE); process.exit(0); }
    else if (a === '-o' || a === '--out') out = args[++i];
    else if (a === '--json') jsonOut = args[++i];
    else if (a === '--from-json') fromJson = args[++i];
    else if (!a.startsWith('-') && !repo) repo = a;
    else die(`unexpected argument: ${a}\n${USAGE}`);
  }

  let payload;
  if (fromJson) {
    payload = JSON.parse(readFileSync(fromJson, 'utf8'));
    repo = payload.repo;
  } else {
    repo ??= repoFromCwd() ?? die(`no repo given and the current directory is not a GitHub repo\n${USAGE}`);
    if (!REPO_RE.test(repo)) die(`invalid repo (expected owner/name): ${repo}`);
    payload = await fetchPayload(repo);
  }

  if (jsonOut) writeFileSync(jsonOut, JSON.stringify(payload, null, 2));

  const outPath = resolve(out ?? `${repo.split('/')[1]}-issues.html`);
  writeFileSync(outPath, renderPage({ title: repo, data: payload }));

  const linked = payload.issues.filter(i => i.parent || i.sub.total > 0).length;
  console.log(`${payload.issues.length} issues (${linked} in hierarchies) → ${outPath}`);
}

/* ---------------- serve mode ---------------- */

async function serveMain(args) {
  let repo = null, port = 8422, host = '127.0.0.1', interval = 30, fromJson = null;
  for (let i = 0; i < args.length; i++) {
    const a = args[i];
    if (a === '-h' || a === '--help') { console.log(USAGE); process.exit(0); }
    else if (a === '-p' || a === '--port') port = Number(args[++i]);
    else if (a === '--host') host = args[++i];
    else if (a === '-i' || a === '--interval') interval = Number(args[++i]);
    else if (a === '--from-json') fromJson = args[++i];
    else if (!a.startsWith('-') && !repo) repo = a;
    else die(`unexpected argument: ${a}\n${USAGE}`);
  }
  if (!Number.isFinite(port) || !Number.isFinite(interval)) die('bad --port or --interval');
  interval = Math.max(1, Math.round(interval));
  const ttlMs = interval * 1000;

  function readFixture() {
    const p = JSON.parse(readFileSync(fromJson, 'utf8'));
    if (!REPO_RE.test(p.repo ?? '') || !Array.isArray(p.issues))
      throw new Error(`${fromJson} is not an issue-graph JSON dump`);
    p.generatedAt = new Date().toISOString();
    return p;
  }

  if (fromJson) repo = readFixture().repo;
  else {
    repo ??= repoFromCwd(); // optional — the index page handles the no-default case
    if (repo && !REPO_RE.test(repo)) die(`invalid repo (expected owner/name): ${repo}`);
  }

  const cache = new Map();    // repo -> {payload, etag, fetchedAt, stale}
  const inflight = new Map(); // repo -> Promise

  async function refresh(r) {
    try {
      const payload = fromJson ? readFixture() : await fetchPayload(r, { quiet: true });
      const etag = `"${createHash('sha1').update(JSON.stringify(payload.issues)).digest('hex')}"`;
      cache.set(r, { payload, etag, fetchedAt: Date.now(), stale: false });
      console.log(`[${new Date().toISOString()}] refreshed ${r}: ${payload.issues.length} issues`);
    } catch (e) {
      console.error(`[${new Date().toISOString()}] refresh failed for ${r}: ${e.message}`);
      const old = cache.get(r);
      if (!old) throw e;
      old.stale = true;
      old.fetchedAt = Date.now(); // back off; serve stale until next TTL
    }
  }

  async function getPayload(r) {
    const c = cache.get(r);
    if (c && Date.now() - c.fetchedAt < ttlMs) return c;
    let p = inflight.get(r);
    if (!p) {
      p = refresh(r).finally(() => inflight.delete(r));
      inflight.set(r, p);
    }
    await p;
    return cache.get(r);
  }

  const esc = s => s.replace(/&/g, '&amp;').replace(/</g, '&lt;');
  function indexPage() {
    const items = [...cache.keys()]
      .map(k => `<li><a href="/${esc(k)}">${esc(k)}</a></li>`).join('');
    return `<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width">
<title>issue-graph</title>
<style>body{font:14px/1.5 system-ui,sans-serif;max-width:460px;margin:80px auto;padding:0 16px;color:#222}
input{font:inherit;padding:6px 9px;width:15em}button{font:inherit;padding:6px 12px}</style>
<h1>issue-graph</h1>
<p>Open <code>/owner/repo</code> for any repository your <code>gh</code> login can see.</p>
<form onsubmit="location.href='/'+this.r.value.trim();return false">
<input name="r" placeholder="owner/repo" autofocus> <button>Open</button></form>
${items ? `<p>Watched so far:</p><ul>${items}</ul>` : ''}`;
  }

  const server = createServer(async (req, res) => {
    const send = (code, body, headers = {}) => {
      res.writeHead(code, { 'Cache-Control': 'no-store', ...headers });
      res.end(body);
    };
    try {
      if (req.method !== 'GET') return send(405, 'method not allowed');
      const u = new URL(req.url, `http://${host}`);
      if (u.pathname === '/') {
        if (repo) return send(302, null, { Location: `/${repo}` });
        return send(200, indexPage(), { 'Content-Type': 'text/html; charset=utf-8' });
      }
      if (u.pathname === '/favicon.ico') return send(204, null);

      const m = u.pathname.match(/^\/([\w.-]+\/[\w.-]+?)(\/data\.json)?$/);
      if (!m) return send(404, 'not found');
      const r = m[1];
      if (fromJson && r !== repo) return send(404, `this server only serves ${repo}`);

      if (m[2]) {
        let entry;
        try { entry = await getPayload(r); }
        catch (e) {
          return send(502, JSON.stringify({ error: e.message }),
            { 'Content-Type': 'application/json' });
        }
        if (!entry.stale && req.headers['if-none-match'] === entry.etag)
          return send(304, null, { ETag: entry.etag });
        const body = entry.stale ? { ...entry.payload, stale: true } : entry.payload;
        return send(200, JSON.stringify(body),
          { 'Content-Type': 'application/json', ETag: entry.etag });
      }

      const html = renderPage({
        title: r,
        live: { repo: r, interval, dataUrl: `/${r}/data.json` },
      });
      return send(200, html, { 'Content-Type': 'text/html; charset=utf-8' });
    } catch (e) {
      send(500, JSON.stringify({ error: e.message }), { 'Content-Type': 'application/json' });
    }
  });

  server.listen(port, host, () => {
    console.log(`issue-graph serving on http://${host}:${port}${repo ? `/${repo}` : ''}` +
      ` (refresh ${interval}s${fromJson ? `, from ${fromJson}` : ''})`);
  });
  if (repo) getPayload(repo).catch(() => {}); // warm the cache; errors surface per-request
}

/* ---------------- dispatch ---------------- */

const argv = process.argv.slice(2);
if (argv[0] === 'serve') await serveMain(argv.slice(1));
else await staticMain(argv);
