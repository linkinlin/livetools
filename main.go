package main

import "C"
import (
	"fmt"
)

//#cgo CFLAGS: -I./ffmpegshared/include
//#cgo LDFLAGS: -L${SRCDIR}/ffmpegshared/bin -lavcodec-59 -lavdevice-59 -lavfilter-8 -lavformat-59 -lavutil-57 -lpostproc-56 -lswresample-4 -lswscale-6
//#include "csource/push.cpp"
/*
extern void logCallback(void*, int, char*, char*);
void setAvLog() {
	av_log_set_level(AV_LOG_INFO);
	av_log_set_callback(logCallback);
	av_log(NULL, AV_LOG_INFO, "ffmeg log callback set success.\n");
}
*/
import "C"

func main() {
	fmt.Println(C.int(C.sum(C.int(1), C.int(2))))
	C.setAvLog()

	C.push()
}
