FROM ruby:3.0.2

WORKDIR /opt/fast_uuidrb

RUN set -ex; \
    apt-get update && \
	apt-get install -y build-essential

ENTRYPOINT [ "./docker-entrypoint.sh" ]
