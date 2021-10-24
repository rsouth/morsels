import matplotlib.pyplot as plt
import numpy as np

import pandas as pd
import json

df = pd.DataFrame({'one': [1, 2, 3], 'two': [3, 2, 1]})

trade_data = {
    'order_id': 12345,
    'arrPx': 102,
    'trdPx': 101,
    'mktBid': 100,
    'mktAsk': 103
}

# invent some mktdata
mkt_data = np.array([95, 98, 97, 98, 98, 99, 101, 101, 99])
time     = np.array([1,   2,  3,  4,  5,  6,   7,   8,  9])
arr_px   = np.array([trade_data['arrPx']])
mkt_px   = np.array([trade_data['mktBid'], trade_data['mktAsk']])
trd_px   = np.array([trade_data['trdPx']])

x = range(95, 105)
y = range(1)
fig = plt.figure()
ax1 = fig.add_subplot(111)

exec_time = np.array([5])

base_pt_size = 40

####
# plt.rcParams.update({'font.sans-serif': "Arial",
#                      'font.family': "sans-serif",
#                      'font.size' : 5,                   
#                     })
####

exec_marker = '$\U0001F352$'  # cherries emoji

ax1.scatter(x=time,                                    y=mkt_data, label='TWC Feed',      marker=".", s=base_pt_size,    c='b')
ax1.scatter(x=exec_time,                               y=arr_px,   label='Arrival Price', marker="*", s=base_pt_size*2,  c='darkorange') 
ax1.scatter(x=exec_time,                               y=trd_px,   label='Fill Price',    marker='x', s=base_pt_size*10)
ax1.scatter(x=np.concatenate([exec_time, exec_time]),  y=mkt_px,   label='TWC Bid/Ask',   marker="_", s=base_pt_size*4,  c='g')

# Trend-line for market data (TWC Feed)
# z = np.polyfit(time, mkt_data, 1)
# p = np.poly1d(z)
# plt.plot(time,p(time),"r--")

# Trend-line for arr px
z = np.polyfit(time, np.full(len(time), arr_px[0]), 1)
p = np.poly1d(z)
plt.plot(time,p(time),"b--")



# trdpx label
for i, label in enumerate(trd_px):
    plt.annotate(label, (time[4], trd_px[i]), rotation=0)
    
plt.legend(loc='lower right')

plt.gcf().set_size_inches((10, 5))    

plt.title("T 0 1/2 2021-21-25 Execution")
# plt.title("Rx: 2021-10-22 16:45:32.123 UTC")
plt.show()

#       | 
#       |               -                    -
#       | -    -  -  -     -           -  -  
#       |   -       *  -      -  -  - *     -
#       |- - - - - - - - - - - - - - - - - - - - <-- Arrival price 
#       | -              *     -  -  -
#       |_ _- _ _ _ _ _ _ _ * _* _ _ _ _ _* _ _ 
#       |                                      *
#       | 
#       | 
# price |_______________________________________
#  time
#           Legend: -      == market data
#                   - - -  == arrival price
#                   *      == child executions
#                   _ _ _  == parent avgpx

# Label the fills with dv01 ?
