## HTTP Server in C++

## Start Docker Container
```sh
docker build -t http-server-cpp . # Build docker image
docker run -it -p 6969:6969 http-server-cpp # Run Docker Container
```
##### The -p 6969:6969 flag maps container port 6969 to your host so you can access it from your browser or curl.

## Build Outside Docker (Optional)

##### Note: Useful if you want compile_commands.json for LSP in VSCode.
```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## Run Server
```sh
./web_server
```

## Connect to Server
```sh
nc localhost 6969
curl.exe "http://localhost:6969"
```
