# Broken Link Finder

## About

`blf` scans a url for broken links. by default, it will follow links 
on the same domain and perform the same analysis on these as well. 
It will work with dynamically generated content and Javascript.


## Dependencies

You will need either 

- [Geckodriver](https://github.com/mozilla/geckodriver/) and [Firefox](https://www.mozilla.org/en-GB/firefox/new/) or 
- [Chromedriver](https://developer.chrome.com/docs/chromedriver/downloads) and [Chromium](https://www.chromium.org/Home/)

in order to use this tool.

## Usage

```
Broken link finder

Usage: blf [OPTIONS] <URL>

Arguments:
  <URL>  The URL to scan.

Options:
  -f, --follow           Follow links and perform analysis on all pages in subdomain. (False by default).
      --cookie <cookie>  Set cookie for requests. Can be specified multiple times.
  -h, --help             Print help
  -V, --version          Print version
```

## Enhancements
- [ ] Extract worker setup into dedicated library.
- [ ] Scan clickable elements, not just hyperlinks.
- [ ] Show xpath of elements which have broken links.
- [ ] Enable setting user agent

## Todo

- [x] Separate scanners into their own modules.
- [x] Scrape a single static page
- [x] Transitive scrape of links within domain (`--follow` option)
- [x] Run JS on page to get link
- [x] Multithread the code
- [x] Scan src attributes as well.

## Learnings

1. Performance analysis with `perf`
2. [Chrome devtools protocol](https://chromedevtools.github.io/devtools-protocol/) vs [webdriver standard](https://www.w3.org/TR/webdriver2/).
3. The Webdriver standard

