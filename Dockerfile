FROM weckyy702/raychel_ci:latest

#Copy source files and set the working directory
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

ARG COMPILER

#Configure step
RUN cmake -DCMAKE_CXX_COMPILER=${COMPILER} -DRAYCHELSCRIPT_BUILD_TOOLCHAIN=ON -DRAYCHELSCRIPT_BUILD_TESTS=ON .
RUN cmake --build . --target all
#TODO: implement unit tests

LABEL Name=raychelscript Version=0.0.1
