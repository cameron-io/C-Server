# C-Server

Using the C 23 standard.

### Requirements
- cmake version >= 3.29
- ninja 1.12.1

### Install Dependencies

MacOS:
```
brew install cmake ninja
```

### Usage

Windows (WSAPoll):
```
.\build.bat
.\.bin\Windows64\bin\Release\WsaPollServer.exe
```

Unix (Select()):

```
make compile
make run APP=HttpServer
```

Alpine Linux - Container (EPoll):
```
make dev
```

### Testing

```
make test
```
