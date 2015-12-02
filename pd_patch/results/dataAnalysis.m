%% DATA ANALYSIS

clc
clear all
close all

% read the data file
%dataPath = '/Users/Paolo/Documents/AAU/2015-2016/Fall Semester Project/MYO-SMC705/pd_patch/results/';
%addpath( dataPath);
fid = fopen( 'tasks.txt');
data = textscan( fid, '%f%f%f%f%f%f%s', 'delimiter', {' ',';'});
fclose(fid);

userID = [ data{1,2} ];
taskID = [ data{1,3} ];
taskLab = [ data{1,4} ];
clicks = [ data{1,5} ];

% add average unlock time at phone.
unlockTime = 3;
taskTime( phoIdx) = taskTime( phoIdx) + unlockTime;

% add 2 clicks for unlock/lock at phone.
clicks( phoIdx) = clicks( phoIdx) + 2;

allData = [userID taskID taskLab clicks taskTime device];

%% separate by userID (to be completed)

totUsers = max( userID);
for i = 1:totUsers
    byUser{i} = allData( find( userID==i), :);
end

%% separate by: taskLabel, userID

totUsers = max( userID);

% MYO
% the following loop search for the task label and then for the user.
% Data are collected into a cell array with {i,j} position, where 'i' is
% the taskLabel and 'j' is the userID.
% Each matrix in the cell contains the data for completed task, that is the
% line for the last attempt of that task.

timeCompletedTask = zeros( 1,6);
for i = 1:5     % select taskLabel
    tempIdx = find( allData_MYO(:,3)==i);
    for j = 1:totUsers      % select userID
        tempIdx2 = find( allData_MYO(:,1)==j);
        myIdx = intersect( tempIdx, tempIdx2);
        for k = 1:20        % select taskID and choose highest time
            tempIdx3 = find( allData_MYO(:,2)==k);
            myIdx = intersect( myIdx, tempIdx3);
            if myIdx ~= 0
                myIdx = find( allData_MYO(:,5) == max( allData_MYO(myIdx,5)));
                timeCompletedTask = [timeCompletedTask; allData_MYO( myIdx, :)];
            end
        end
    end
end

timeCompletedTask(1,:) = [];    % removing first row of zeros.

%% single task plot
% here we plot taskTime vs number of attempt, each point is the average
% time for that task among the users for attempts from 1 to 5.

figure(plotTask1_MYO)
plot( byTask_MYO{1}

%%
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