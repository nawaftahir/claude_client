FROM cod2-msvc71-vs2003
USER root
RUN apt-get update && apt-get install -y --no-install-recommends faketime libfaketime && rm -rf /var/lib/apt/lists/* && find / -name 'libfaketime.so.1' 2>/dev/null
