## Control Docker



### Give execute permission to **`control_docker.sh`**

```console
$ chmod +x ./docker/docker.sh
```

### Get available commands
```console
$ ./docker/docker.sh help
Available commands: 
 run 
 lscon 
 rm 
 build 
 rm-image 
 lsimg 
```

### Build the project image
Go to the docker directory and build the image

```console
$ cd docker/ && ./docker.sh build
```

Go back to the main directory
```console
$ cd ..
```

### List active images

```console
$ ./docker/docker.sh lsimg
REPOSITORY       TAG           IMAGE ID       CREATED        SIZE
dsat             1.0           0fbd6842a768   2 days ago     1.15GB
```

### Run the project

```console
$ ./docker/docker.sh run
root@e698a84bf24c:/dsat#
```

### List active containers

```console
$ ./docker/docker.sh lscon
CONTAINER ID   IMAGE       STATUS                      NAMES
e698a84bf24c   dsat:1.0    Exited (0) 5 seconds ago    dsat
```

### Remove project container

```console
$ ./docker/docker.sh rm
dsat
```

### Remove project image
First, `remove the project container`, then:
```console
$ ./docker/docker.sh rm-image
Untagged: dsat:1.0
Deleted: sha256:0fbd6842a76814f3be3727c7d3e7585a361562cfa17907aed11425f0faf02367
Deleted: sha256:2c45ce425c3d718efc1c664eea116b64e6225912b0f37f12bd41d3bb3917685a
Deleted: sha256:7a1271751c3fd60b4b1db18eb858fc27c8f989e6ac574df3a2e7db6e7437d102
Deleted: sha256:17566549f6d4eb2ea0bcd4c4e76c74722640cc64534ab5fd2e17cc25e168d970
Deleted: sha256:f093af0a3b357ec9bca6bf1ff018391260a3befa1e3de0a3a5b1180f9c3d86a5
Deleted: sha256:5921ec2934fea1bba2a73d3bbc214e47bd0b59579ba4a95580ebfcd57e0478ec
```