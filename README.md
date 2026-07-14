# issue-graph

Render a GitHub repo's issue hierarchy (sub-issues) as a self-contained,
interactive HTML graph — issues and their children, laid out as trees.

```
issue-graph bricef/factor-q
# → factor-q-issues.html   (open it in any browser)
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
issue-graph [owner/repo] [options]

  -o, --out FILE     output path (default: <repo>-issues.html)
      --json FILE    also dump the fetched issue data as JSON
      --from-json F  render from a previously dumped JSON file (no fetching)
  -h, --help
```

With no repo argument it uses the GitHub repo of the current directory.

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
