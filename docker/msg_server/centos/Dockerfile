FROM centos:centos7.9.2009 AS base
RUN yum -y install apr-devel apr-util-devel net-tools
WORKDIR /teamtalk
EXPOSE 8000

FROM 1569663570/centos792009_develop_server AS builder

FROM base AS runtime
COPY --from=builder /lib64/libstdc++.so.6 /lib64/libstdc++.so.6
COPY --from=builder /develop/im-server-1.0.0/msg_server /teamtalk/msg_server
COPY --from=builder /develop/im-server-1.0.0/daeml /teamtalk/daeml
COPY ./server/run/monitor.sh /teamtalk/msg_server/monitor.sh
COPY ./server/run/restart.sh /teamtalk/restart.sh
COPY ./docker/msg_server/main.sh main.sh
RUN chmod +x /teamtalk/main.sh
ENTRYPOINT ["/teamtalk/main.sh"]
CMD ["/bin/bash"]