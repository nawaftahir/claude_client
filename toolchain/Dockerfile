# MSVC 7.1 (cl 13.10.3077, link 7.10.3077) under Wine 5.0 (bullseye), plus make.
# Build with ./build.sh (assembles msvc710.tar.gz from the VS2003 discs, see README.md).
# Do not use bookworm: Wine 8 makes cl.exe spin forever on any compile error.
FROM i386/debian:bullseye-slim
RUN sed -i "/security/d" /etc/apt/sources.list && apt-get update \
 && apt-get -o Acquire::Retries=5 install -y --no-install-recommends wine make \
 && rm -rf /var/lib/apt/lists/*
ENV WINEDEBUG=-all
ENV INCLUDE=Z:\\opt\\msvc710\\Vc7\\include;Z:\\opt\\msvc710\\Vc7\\PlatformSDK\\Include
ENV LIB=Z:\\opt\\msvc710\\Vc7\\lib;Z:\\opt\\msvc710\\Vc7\\PlatformSDK\\Lib
ADD msvc710.tar.gz /opt/
WORKDIR /work
RUN wineboot -i >/dev/null 2>&1 || true
CMD ["/bin/bash"]
