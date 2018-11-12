TARGET=breakout_client breakout_server breakout_server_v2

all:$(TARGET)

%:%.cpp
	g++ -o $@ $^ -lpthread

clean:
	rm -rf *.o $(TARGET)

