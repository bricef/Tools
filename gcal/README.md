# Google Automation tools

## About

This is a collection of useful tools for interacting with google cloud services (Calendar, Mail, etc...) via the command line for the purpose of automation.

## Tools

- `gcal` calendar automator downloads events from your google calendar in a markdown format compatible with [Obsidian](https://obsidian.md/).

By default `gcal` shows the current work week. Use the flags below to change the window:

- `-n`, `--ndays <N>`: number of days to show (default `7`). When passed without `--start`, the window begins today.
- `-s`, `--start <YYYY-MM-DD>`: start date for the window, e.g. `gcal --start 2026-03-23`. Combine with `--ndays` to control the length. Works for past dates as well as future ones.

## Install

`gcal` can be installed with `go install` to the default `$GOBIN` path.


## Todo

- [x] Allow taking numdays parameter
