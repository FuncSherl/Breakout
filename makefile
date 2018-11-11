TARGET=breakout_client breakout_server

all:$(TARGET)

%:%.cpp
	g++ -o $@ $^ -lpthread

clean:
	rm -rf *.o $(TARGET)

