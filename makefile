TARGET=breakout_client breakout_server

all:$(TARGET)

%:%.cpp
	g++ -o $@ $^

clean:
	rm -rf *.o $(TARGET)

