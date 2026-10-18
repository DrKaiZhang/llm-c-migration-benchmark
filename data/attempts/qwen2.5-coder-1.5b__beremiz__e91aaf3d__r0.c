/* model=qwen2.5-coder-1.5b task=beremiz__e91aaf3d rep=0 */
#define FSAFromStatusWord(SW) (SW & 0x006f)

#define HomingStatusWord(SW) (SW & 0x3400)
#define FaultFromStatusWord(SW) (SW & 0x0008)
#define NotReadyToSwitchOn  0b00000000 FSA_sep 0b00100000
#define SwitchOnDisabled    0b01000000 FSA_sep 0b01100000
#define ReadyToSwitchOn     0b00100001
#define SwitchedOn          0b00100011
#define OperationEnabled    0b00100111
#define QuickStopActive     0b00000111
#define FaultReactionActive 0b00001111 FSA_sep 0b00101111
#define Fault               0b00001000 FSA_sep 0b00101000


#define HomingInProgress	0b0000000000000000
#define HomingNotRunning	0b0000010000000000
