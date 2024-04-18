# Brice's Doctools

This repository contains tools for dealing with documents. Mostly, for turning various flavours of Markdown into PDF documents.

## Structure and organisation

The `bin` repo contains the actual scripts, whilst the `tools` repo contains the code, makefiles, buildfiles, etc.. for each tool.

## Tools

`dockerised-pandoc` is a script that invokes pandoc through a docker container, to make dependency management for the various PDF backends eaasier.

`pinc.py` is a small script that will parse a text file for shell commands and special include statements to enable text inclusion and build more complex documents. I've found it to be a useful little script when collating multiple markdown files together before processing them with another tool like pandoc.

## Future work

- [ ] Rebuild pinc to use [Handlebars](https://handlebarsjs.com/) for a more complete and expressive option. In fact already has [the read file helper](https://github.com/helpers/handlebars-helpers#read).
- [ ] Build a generic 'doctools' image that contains all doctools and let the scripts here be aliases for different tools in the image

## Notes and prior art

- https://pbpython.com/pdf-reports.html
- https://github.com/dalibo/pandocker
- https://github.com/Wandmalfarbe/pandoc-latex-template
- https://github.com/aaronwolen/pandoc-letter
- https://github.com/Wandmalfarbe/pandoc-latex-template