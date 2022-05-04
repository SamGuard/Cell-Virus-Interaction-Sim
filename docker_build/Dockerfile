FROM ubuntu:latest
RUN apt update
RUN yes | apt install g++ gcc make zlib1g-dev wget tar
RUN wget https://github.com/Repast/repast.hpc/releases/download/v2.3.1/repast_hpc-2.3.1.tgz
RUN tar -xzvf repast_hpc-2.3.1.tgz
RUN cp -r repast_hpc-2.3.1 /root/
WORKDIR /root/repast_hpc-2.3.1/MANUAL_INSTALL/
RUN rm Makefile
COPY Makefile .
RUN ./install.sh curl
RUN ./install.sh mpich
ENV PATH=/root/sfw/MPICH/bin:$PATH
RUN ./install.sh netcdf
RUN ./install.sh boost
RUN ./install.sh rhpc
ENV LD_LIBRARY_PATH=/root/sfw/repast_hpc-2.3.1/lib/:/root/sfw/boost-1_61_0/lib/:$LD_LIBRARY_PATH
RUN yes | apt install git libsfml-dev python3
WORKDIR /root/
RUN git clone https://github.com/SamGuard/Cell-Virus-Interaction-Sim.git
WORKDIR /root/Cell-Virus-Interaction-Sim/
RUN rm env
COPY env env
RUN mkdir objects; exit 0
RUN mkdir bin; exit 0
RUN mkdir output; exit 0
CMD git pull \ 
  && make clean \
  && make build_opt \
  && make run \
  && bash