<div align="center">
	<h3>WBTB — Web Binary Terminal Browser</h3>
</div>

**WBTP** is a drop-in replacement for **HTTP** (but not **HTTPS**). However, readable text was swapped out for packed binary! This allows for quicker transfer speeds and serialization/deserialization, at the loss of direct readability.

This project depends on the [WBTP CLI](https://github.com/gh-wbtp/cli) to provide a document browser for the [WBL (Web Binary Language)](#wbl-spec) format. You can find documentation for the CLI by visiting [this section](#cli-documentation).

The compiled AST/"DOM" tree renders at ~60 FPS in an 80x25 framebuffer, which then gets copied to the host processes's `stdout`. This should be viewable in the terminal you used to launch it.

Each release package provides native binaries/executables for one of [5 platforms](#supported-platforms-5), along with licensing and this very same README!

---

### Table of Contents
- [Installation](#installation)
- [CLI Documentation](#cli-documentation)
- [WBL Spec](#wbl-spec)
- [Supported Platforms (5)](#supported-platforms-5)

---

### Installation

> **GUI installer coming to Win(slop|dows) soon!** For now, extract the release ZIP and place the **EXE** in some location on your **PATH**. This installation section is meant only for Unix-like systems.

We have a remote sh/bash script you can execute directly! It automatically picks a package for your OS/arch, downloads it, extracts it, and places the binaries where they need to go. It's that easy!

```bash
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/wbtb/main/script/install.sh | sh              # User-local
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/wbtb/main/script/install.sh | sh -s -- --sys  # System-wide
```

**Suppose you get tired of us...** you're cooked? Nope! Actually you are, 'cus you do in fact need it to survive. But we also provided an uninstallation script for the weirdos :)

```bash
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/wbtb/main/script/uninstall.sh | sh              # User-local
curl -fsSL https://raw.githubusercontent.com/gh-wbtp/wbtb/main/script/uninstall.sh | sh -s -- --sys  # System-wide
```

---

### CLI Documentation

Quick notes:
- No args need to be passed.
- Anything put into `stdin` will be tokenized, parsed, and rendered as a [WBL (Web Binary Language)](#wbl-spec) document. This aligns with Unix-style piping!
- When `wbtp` is run with `--raw` mode, the output is perfectly compatible with `wbtb`.

(Working) example usage:
```bash
wbtp -r wbtp.flappygrant.com | wbtb
```

---

### WBL Spec

You can think of `WBL` as a cross between HTML and original XML, because it kinda is. You don't get any attributes just tags! Tags and text. Truly all you need (I hope).

Example tag:
```xml
<h1>Moo!</h1>
```

Undefined tags do not have any special properties. To put it simply, our parser walks through the whole thing to look for text! For defined tags, however, they modify a global state which only affects the contents of that tag. Oh and all tags require a closing one because even though I like self-closing tags, they don't fit with my minimal spec!

Defined tags may include but are not limited to (because I always forget to update the README):
- `<h1> <h2> <h3>` all do nothing (for now)
- `<bold> <italic> <underline>` apply their respective styles to their contents
- `<#??????>` is syntax for a potential color tag (not implemented yet BECAUSE MY SERVER CAN'T SEEM TO HANDLE SENDING A 40 BYTE PAYLOAD EVEN THOUGH IT COULD SEND A SEVERAL-KIBIBYTE IMAGE IWAHFAWKJFWAJKFWAHFWAKJ)

Nesting works for all tags. The chain of nesting ends when there's only text nodes left, so beware of how much nesting you have! The more text compared to elements, the better.

Example of nesting:
```xml
<center>
	<h1>Even though h1 doesn't center me, I am still centered because of my parent tag!</h1>
</center>
```

---

### Supported Platforms (5)

- Windows (x86_64 + arm64)
- Linux (x86_64 + arm64)
- macOS (arm64)
