#pragma once

// IID_PPV_ARGS()をWinRTに対応させるマクロ
#define IID_PPV_ARGS_WRT(ppType) __uuidof(ppType), ppType.put_void()