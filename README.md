# Breakout
目标：内网穿透<br>
需求：一台公网服务器(需求公网ip)<br>
注：仅限于Linux系统<br>

现有clientA、clientB、server，想要clientA连内网中的clientB，以ssh为例:<br>
1、makefile目录下执行make<br>
2、在server上运行  breakout_server_v2  ,不需要参数，需要修改参数可以修改common.h里的#define XXX<br>
3、在clientB上运行   breakout_client_v2 serverip serverport  localport<br>
其中serverip就是服务器公网ip，serverport就是服务器映射的端口，默认下在7001-7050（common.h里面可以修改），<br>这个端口这里可以任选，但是要和待会ssh里设置的端口一致，localport就是想要将clientB中哪个端口映射出去，如ssh就是22<br>
4、假如localport=7001，则在clientA上运行ssh username@serverip -p 7001<br>

出现错误： message authentication code incorrect就多重连2次就好 <br>
enjoy<br>


本工程在实现时较为简单，为临时启用，很多边界条件处理不好

后续在另一工程中用python重写了内网穿透功能： `https://github.com/functionxu123/RespPiPlays/` 该实现配置性和完备性较好

虽然内网穿透功能在此工程中只占一部分，但是独立出来作为一个单独的功能也是可以的，具体查看：`https://github.com/functionxu123/RaspPiPlays/tree/master/0001_CamMonitor#2--`