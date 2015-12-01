%% DATA ANALYSIS

clc
clear all
close all

% read the data file
fid = fopen('tasks.txt');
data = textscan( fid, '%f%f%f%f%f%f%s', 'delimiter', {' ',';'});
fclose(fid);

userID = [ data{2} ];
taskID = [ data{3} ];
taskNum = [ data{4} ];
clicks = [ data{5} ];
taskTime = [ data{6} ];

% MYO = 1  |  PHONE = 0
device = [ double( strcmp( data{7}, 'myo' )) ];

myoIdx = find( device==1 );
phoIdx = find( device==0 );

% add average unlock time at phone.
unlockTime = 3;
taskTime( phoIdx) = taskTime( phoIdx) + unlockTime;

% add 2 clicks for unlock/lock at phone.
clicks( phoIdx) = clicks( phoIdx) + 2;

% divide myo and phone data
myo_userID = userID( myoIdx);
myo_taskID = taskID( myoIdx);
myo_clicks = clicks( myoIdx);
myo_taskTime = taskTime( myoIdx);

pho_userID = userID( phoIdx);
pho_taskID = taskID( phoIdx);
pho_clicks = clicks( phoIdx);
pho_taskTime = taskTime( phoIdx);

% userID and taskTime (MYO)
user1idx = find( myo_userID == 5);
figure(5)
plot( taskID( user1idx), taskTime( user1idx), '.', 'MarkerSize', 20 )

%% PLOT TASK TIME (MYO & PHONE)

% NOTE: we can make the average time for each task on the myo and for each
% task on the phone, and plot them. The same for number of clicks.

figure(1)
plot( myo_taskTime, '.', 'MarkerSize', 20);
hold on
plot( myo_taskTime);
hold on
plot( pho_taskTime, '.', 'MarkerSize', 20);
hold on
plot( pho_taskTime);