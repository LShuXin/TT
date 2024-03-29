# TeamTalk Docker 版本

## 直接从已有镜像运行 TeamTalk 容器

```Dockerfile
sudo docker-compose -f docker-compose-centos792009.yml up -d
# 尚未完成
# sudo docker-compose -f docker-compose-openeuler2003.yml up -d
```

## 本地构建 TeamTalk 镜像，然后再运行 TeamTalk 容器

```Dockerfile
sudo docker-compose -f "docker-compose-build-centos792009.yml" up -d --build
# 尚未完成
# sudo docker-compose -f "docker-compose-build-openeuler2003.yml" up -d --build
```

>version: "3.8"：这一行指定了 Docker Compose 文件的版本。版本号 3.8 是 Docker Compose 文件格式的版本，它决定了文件的语法和支持的功能。
>
>services:：这是一个服务定义的部分，其中包含了多个服务的配置信息。
>
>develop_server：这个服务定义了一个容器，它使用了名为 1569663570/openeuler_develop_server 的镜像。它的配置包括构建 Docker 镜像的上下文路径、Dockerfile 文件、容器的重启策略、容器名称、主机名、网络设置、时区环境变量和额外的主机名映射。此服务是应用程序的核心开发服务器。
>
>web_server：这个服务定义了一个容器，它使用了名为 1569663570/openeuler_web_server 的镜像。它与 develop_server 服务有依赖关系，并映射了容器端口到主机端口。此服务用于提供 Web 服务。
>
>login_server、msg_server、db_proxy_server、route_server、http_msg_server、push_server、file_server 和 msfs_server：这些服务定义了不同的容器，它们分别用于不同的应用组件，如登录服务器、消息服务器、数据库代理服务器等。它们具有类似的配置结构，包括镜像、构建、重启策略、容器名称、主机名、网络设置、端口映射、环境变量和依赖关系。
>
>mariadb_server：这个服务定义了一个容器，它使用了名为 lsqtzj/openeuler_mariadb 的镜像。该容器与主机共享数据卷，用于存储 MySQL 数据，并设置了一些环境变量来配置 MariaDB 数据库服务器。
>
>redis_server：这个服务定义了一个容器，用于运行 Redis 服务器。它设置了容器的网络配置和环境变量。
>
>networks:：这是网络定义的部分。它定义了名为 teamtalk 的 Docker 自定义网络，并指定了该网络的子网和网关。每个服务都连接到这个网络，并分配了一个独特的 IPv4 地址，以便容器之间可以相互通信。
>
>extra_hosts:：这个部分定义了额外的主机名到 IP 地址的映射。这对于容器内的应用程序可以使用主机名来访问其他容器很有用，而不必依赖于 IP 地址。
>
>depends_on:：这个部分定义了容器之间的依赖关系。例如，某些容器依赖于其他容器，这意味着当某个容器启动时，它会等待依赖的容器先启动。这有助于确保容器以正确的顺序启动。
>
>volumes:：这个部分定义了容器与主机之间的卷挂载，用于数据持久化。例如，mariadb_server 服务使用了卷挂载来存储 MySQL 数据。
>
>总的来说，这个 Docker Compose 文件定义了一个复杂的多容器应用程序，每个容器负责不同的任务，它们通过定义的网络和主机名相互通信，使用环境变量进行配置，通过依赖关系确保正确的启动顺序，并通过卷挂载来实现数据持久化。这种容器化的方式使得应用程序的部署和维护更加容易管理。