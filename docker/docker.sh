#!/usr/bin/env bash

function env_up {
    local file_folder="$(dirname "$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )")"

    if [ -f "$file_folder/.env" ]; then
        set -a
        source "$file_folder/.env"
        set +a
    else
        printf "ERROR: .env file not found in project root \n"
        return 1
    fi
}

function rm {
    docker rm ${CONTAINER_NAME}
}

function rm-image {
    docker image rm ${DOCKER_IMAGE}
}

function run {
    if [ ! "$(docker ps -aq -f name=^${CONTAINER_NAME})" ]; then # https://stackoverflow.com/questions/38576337
        docker run --name ${CONTAINER_NAME} --security-opt seccomp:unconfined -v "${PWD}:/${VOLUME_NAME}" -ti ${DOCKER_IMAGE}
    else
        docker start --interactive ${CONTAINER_NAME}
    fi    
}

function build {
    docker build --build-arg volume_name=${VOLUME_NAME} -t ${DOCKER_IMAGE} .
}

function list_containers {
    docker ps --last=10 --format="table {{.ID}}\\t{{.Image}}\\t{{.Status}}\\t{{.Names}}"
}

function list_images {
    docker images
}

function help {
    printf "Available commands: \n run \n lscon \n rm \n build \n rm-image \n lsimg \n help \n"
}

function main () {
    env_up

    case "$1" in
        build)
            build
            ;;
        run)
            run
            ;;
        rm)
            rm
            ;;
        rm-image)
            rm-image
            ;;
        lscon)
            list_containers
            ;;
        lsimg)
            list_images
            ;;
        help)
            help
            ;;    
        *)
            printf "ERROR: Missing command. \n"
            help
            exit 1
            ;;
    esac
}

main "$@"
