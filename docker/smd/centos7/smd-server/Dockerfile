FROM centos:centos7

RUN yum install -y make gcc git
RUN git clone -v0.0.1 https://github.com/efiop/smd.git
WORKDIR ./smd
RUN make
CMD ./smd server
