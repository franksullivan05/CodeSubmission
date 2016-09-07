pio: 

timer: 

pacer: timer

navswitch: pio

task: timer

led: pio

control: pio pacer navswitch task led

display: pio

paddle: pio display

ball: pio paddle display

game: pio pacer navswitch task led control paddle ball display

