# Ubuntu 20 development image

A quick and dirty Docker image to use in the OS workshop.  To build:


```
cd environments/ubuntu
DOCKER_BUILDKIT=1 docker build --progress=plain --ssh default -t osworkshop .
```

Note that the buildkit features map your SSH agent into the building container to do the git checkouts.

