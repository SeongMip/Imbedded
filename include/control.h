#ifndef CONTROL_H
#define CONTROL_H

// 0..100 범위의 “밝기”를 시스템 공용 인터페이스로 제공
void control_set_brightness(int pct_0_100);
int  control_get_brightness(void);

#endif
