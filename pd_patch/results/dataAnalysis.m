%% DATA ANALYSIS

clc
clear all
close all

% read the data file
fid = fopen('tasks.txt');
data = textscan( fid, '%f%f%f%f%f%s', 'delimiter', {' ',';'});
fclose(fid);

userID = [ data{2} ];
taskID = [ data{3} ];
clicks = [ data{4} ];
taskTime = [ data{5} ];

% MYO = 1  |  PHONE = 0
device = [ double( strcmp( data{6}, 'myo' )) ];

myoIdx = find( device==1 );
phoIdx = find( device==0 );

myo_userID = userID( myoIdx);
myo_taskID = taskID( myoIdx);
myo_clicks = clicks( myoIdx);
myo_taskTime = taskTime( myoIdx);

pho_userID = userID( phoIdx);
pho_taskID = taskID( phoIdx);
pho_clicks = clicks( phoIdx);
pho_taskTime = taskTime( phoIdx);

%% PLOT TASK TIME (MYO & PHONE)

% NOTE: we can make the average time for each task on the myo and for each
% task on the phone, and plot them. The same for number of clicks.

figure(1)
plot( myo_taskTime, '.', 'MarkerSize', 20);
hold on
plot( pho_taskTime, '.', 'MarkerSize', 20);