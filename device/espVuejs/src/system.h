#include "webserver.h"
void setupSystem(){
    addHttpApi("/reset/",[](){
        server.send(200);
        delay(500); // chờ send thành công.
        ESP.reset();
    });
    addHttpApi("/reset2/",[](){
        server.send(200);
    });
}