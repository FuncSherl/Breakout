TARGET=breakout_client breakout_server breakout_server_v2 breakout_client_v2

all:$(TARGET)

%:%.cpp common.h
	g++ -o $@ $^ -lpthread

clean:
	rm -rf *.o $(TARGET)

