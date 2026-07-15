# issue-graph

Render a GitHub repo's issue hierarchy (sub-issues) as a self-contained,
interactive HTML graph — issues and their children, laid out as trees.

```
issue-graph bricef/factor-q
# → factor-q-issues.html   (open it in any browser)

issue-graph serve bricef/factor-q
# → live view on http://127.0.0.1:8422  (auto-refreshes every 30s)
```

No dependencies. Uses `gh` for authentication, so it works on any repo your
`gh` login can see, public or private.

## What you get

A single HTML file (data embedded, works offline, shareable) showing:

- **Trees** — each parent issue with its sub-issues fanned out left-to-right;
  parents show a completion ring (`8/18`) from GitHub's own sub-issue summary,
  which counts children in *other* repos too.
- **State at a glance** — GitHub's icon + colour conventions: green circle-dot
  open, purple check done, grey slash not planned / duplicate. Legend with
  counts in the header.
- **Hover** for full title, labels, assignees, milestone, type, and dates;
  **click** to open the issue on GitHub.
- **Collapse/expand** subtrees; collapsed parents show a `+n` badge.
- **Filter** by number, title, label, or assignee (non-matches dim).
- **Hide completed** — prunes fully-closed subtrees so only remaining work shows.
- **Unlinked issues** — issues with no parent and no children live behind a
  toggle so hierarchy stays the focus.
- Pan (drag), zoom (wheel / buttons), fit-to-view, light/dark theme.

## Usage

```
issue-graph [owner/repo] [options]          render a static HTML file
issue-graph serve [owner/repo] [options]    live-updating local server

static:
  -o, --out FILE     output path (default: <repo>-issues.html)
      --json FILE    also dump the fetched issue data as JSON
      --from-json F  render from a previously dumped JSON file (no fetching)

serve:
  -p, --port N       port (default: 8422)
      --host ADDR    bind address (default: 127.0.0.1)
  -i, --interval N   refresh interval in seconds (default: 30)
      --from-json F  serve a JSON dump, re-read on every refresh (no gh)
```

With no repo argument either mode uses the GitHub repo of the current
directory.

## Live mode

`issue-graph serve` polls GitHub on the interval (a whole repo is one GraphQL
query, a few rate-limit points per hundred issues against a 5,000/hour
budget) and the page polls the server, so the graph stays current while you
work:

- Nodes whose state changed **pulse**; new issues appear in place — your
  pan/zoom, collapsed subtrees, and filters are preserved across updates.
- A **live badge** in the header turns amber when GitHub is unreachable
  (stale data, timestamped) and red when the server is gone; it recovers by
  itself.
- ETag/304 end to end: nothing re-renders when nothing changed.
- `/` serves an index of watched repos (or redirects to the default repo);
  any `/owner/repo` path is fetched on first visit.

The server binds to **127.0.0.1** and has **no auth** — it will serve
anything your `gh` login can see to anyone who can reach the port. Widen
`--host` only behind something that authenticates for you.

## Requirements

- `node` ≥ 18
- `gh` (authenticated), unless rendering with `--from-json`

## Notes

- Fetches **all** issues, open and closed (paginated, 100/page); filtering
  happens client-side in the generated page.
- A child whose parent lives in another repository is shown as a root with a
  `↗ child of owner/repo#N` flag — cross-repo *children* of local parents
  still count in the completion ring but aren't drawn.
- Issue data is baked into the HTML at generation time; re-run to refresh
  (the header shows the generation timestamp).
