FROM ubuntu:latest

RUN apt update && \
    apt install -y \
        xvfb mesa-utils \
        cmake \
        build-essential \
        qt6-declarative-dev \
        qml6-module-qtquick \
        qml6-module-qtquick-controls \
        qml6-module-qtqml-workerscript \
        qml6-module-qtquick-templates

WORKDIR /app
COPY . .
RUN cmake -B build -S .
RUN cmake --build build --parallel --target install

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
CMD ["/entrypoint.sh"]
