FROM docker.io/ocaml/opam:debian-ocaml-4.08 as build
WORKDIR /src
ENV ELKHOUND_BUILD_DIR /src/obj/elkhound
ENV ELKHOUND_SRC_DIR /src/elkhound-master/src
ENV PATH "${PATH}:/home/opam/.opam/4.08/bin"
USER root
RUN apt-get update -yqqq && \
    apt-get install -yqqq bison cmake flex wget && \
    wget https://github.com/WeiDUorg/elkhound/archive/refs/heads/master.zip && \
    unzip master.zip && \
    mkdir -p ${ELKHOUND_BUILD_DIR} && \
    cmake -Wno-dev -S ${ELKHOUND_SRC_DIR} -B ${ELKHOUND_BUILD_DIR} -D CMAKE_BUILD_TYPE=Release && \
    make -C ${ELKHOUND_BUILD_DIR}
# Weidu
FROM docker.io/ocaml/opam:debian-ocaml-4.08
COPY --from=build /src/obj/elkhound/elkhound/elkhound /usr/bin/elkhound
WORKDIR /src
COPY . .
USER root
RUN chown -R opam:opam /src
USER opam
RUN mv docker.Configuration Configuration && \
    mkdir -p obj/x86_LINUX obj/.depend && \
    make
