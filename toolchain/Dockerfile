# MSVC 7.1 (cl 13.10.3077, link 7.10.3077) under Wine, plus make.
# Base image cod2-msvc71 is built from a local copy of the Visual C++ .NET 2003 install.
FROM cod2-msvc71
RUN apt-get update && apt-get install -y --no-install-recommends make && rm -rf /var/lib/apt/lists/*
RUN wineboot -i >/dev/null 2>&1 || true
