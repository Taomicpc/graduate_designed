[<c009a668>] (cpu_idle+0x0/0x84) from [<c04e45b8>] (rest_init+0x60/0x78)        
     r5:c0027eac r4:c06c0be4                                                        
     [<c04e4558>] (rest_init+0x0/0x78) from [<c0008990>] (start_kernel+0x280/0x2cc)  
    [<c0008710>] (start_kernel+0x0/0x2cc) from [<50008040>] (0x50008040)            
    BUG: scheduling while atomic: swapper/0/0x40000100                              
    Modules linked in: taoTower rt3070sta libertas_sdio libertas rt73usb rt2x00usb ]
                                                                                    
    Pid: 0, comm:              swapper                                              
    CPU: 0    Not tainted  (3.0.0-rc6-EmbedSky #2)                                  
    PC is at cpu_idle+0x48/0x84                                                     
    LR is at cpu_idle+0x48/0x84                                                     
    pc : [<c009a6b0>]    lr : [<c009a6b0>]    psr: 60000013                         
    sp : c06bffa0  ip : c06bffa0  fp : c06bffb4                                     
    r10: 50026000  r9 : 410fb766  r8 : 50004008                                     
    r7 : c06c4cfc  r6 : c09778a0  r5 : c0701a84  r4 : c06be000                      
    r3 : 00000000  r2 : c06bff90  r1 : 00000000  r0 : ccf3e480                      
    Flags: nZCv  IRQs on  FIQs on  Mode SVC_32  ISA ARM  Segment kernel             
    Control: 00c5387d  Table: 5cf7c008  DAC: 00000017                               
    [<c009a994>] (show_regs+0x0/0x50) from [<c04e8a64>] (__schedule_bug+0x50/0x64)  
     r4:c06bff58 r3:60000113                                                        
     [<c04e8a14>] (__schedule_bug+0x0/0x64) from [<c04ec50c>] (schedule+0x4c/0x3cc)  
     r4:c06c3cf8 r3:00000000                                                        
     [<c04ec4c0>] (schedule+0x0/0x3cc) from [<c00ac278>] (__cond_resched+0x28/0x38)  
    [<c00ac250>] (__cond_resched+0x0/0x38) from [<c04eca80>] (_cond_resched+0x38/0x)
     r4:00000079 r3:00000100                                                        
     [<c04eca48>] (_cond_resched+0x0/0x48) from [<c028e438>] (gpio_free+0x14/0xdc)   
    [<c028e424>] (gpio_free+0x0/0xdc) from [<c00a5eb8>] (s3c_gpio_setpin+0x30/0x38) 
    [<c00a5e88>] (s3c_gpio_setpin+0x0/0x38) from [<bf15e138>] (time_list_handle+0x3)
     r5:bf15e5a4 r4:00000001                                                        
     [<bf15e104>] (time_list_handle+0x0/0x78 [taoTower]) from [<c00be78c>] (run_time)
     r4:c0724200 r3:c0700f24                                                        
     [<c00be598>] (run_timer_softirq+0x0/0x2d4) from [<c00b79d0>] (__do_softirq+0xc0)
    [<c00b7910>] (__do_softirq+0x0/0x194) from [<c00b7de8>] (irq_exit+0x48/0x50)    
    [<c00b7da0>] (irq_exit+0x0/0x50) from [<c008b06c>] (asm_do_IRQ+0x6c/0x8c)       
    [<c008b000>] (asm_do_IRQ+0x0/0x8c) from [<c0099154>] (__irq_svc+0x34/0x80)      
    Exception stack(0xc06bff58 to 0xc06bffa0)                                       
    ff40:                                                       ccf3e480 00000000   
    ff60: c06bff90 00000000 c06be000 c0701a84 c09778a0 c06c4cfc 50004008 410fb766   
    ff80: 50026000 c06bffb4 c06bffa0 c06bffa0 c009a6b0 c009a6b0 60000013 ffffffff   
     r5:f6000000 r4:ffffffff 
