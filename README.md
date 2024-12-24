# RemDeb
A library (or a standalone application) that is ran/loaded inside an application.

Client-server communications happens on a Socket at port 8088 (can be configured in code)

# Building
```bash
mkdir b && cd b
meson
```

> For cross-compiling, just pass a crossfile file using the '--cross-file \<crossfile>' argument through 'meson'
> Cross-compiling may or may not succeed at first try. Further configurations are probably required.