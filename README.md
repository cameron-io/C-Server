# C-Server

### Requirements
- cmake version >= 3.29
- ninja 1.12.1

### Install Dependencies

MacOS:
```
brew install cmake ninja
```

### Usage

Multi-Platform (libuv):
```
make compile
make run APP=HttpServer
```

Alpine Linux (Container):
```
make dev
```

### Testing

```
make test
```
