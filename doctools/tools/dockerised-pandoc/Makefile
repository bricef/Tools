

GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
GIT_SHA := $(shell git rev-parse --short HEAD)

IMAGE := quay.io/brice/weasyprint
TAG := ${GIT_BRANCH}-${GIT_SHA}

build:
	docker build -t ${IMAGE}:${TAG} .

push: 
	docker push ${IMAGE}:${TAG}

release:
	docker build -t ${IMAGE}:${TAG} .
	docker push ${IMAGE}:${TAG}
	docker build -t ${IMAGE}:latest .
	docker push ${IMAGE}:latest
