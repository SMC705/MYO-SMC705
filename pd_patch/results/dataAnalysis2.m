%% DATA ANALYSIS

clc
clear all
close all

%% Load data

% read the data file
%dataPath = '/Users/Paolo/Documents/AAU/2015-2016/Fall Semester Project/MYO-SMC705/pd_patch/results/';
%addpath( dataPath);
fid = fopen( './tasks_CLEANED.txt');
data = textscan( fid, '%f%f%f%f%f%f%s', 'delimiter', {' ',';'});
fclose(fid);

% dataset matrix: userID | taskID | taskLab | click | time | device(myo=1,pho=2)
dataset = [ data{2} data{3} data{4} data{5} data{6} strcmp( data{7}, 'myo') ];

% DEFINE INDICES:
% to select a particular combination of data, intersect the
% indices and extract the data of interest from 'dataset'.
totUsers = max( dataset(:, 1));
for i = 1:totUsers
    idx_user{i} = find( dataset(:, 1) == i );
end
for i = 1:20
    idx_taskID{i} = find( dataset(:, 2) == i );
end
for i = 1:5
    idx_taskLab{i} = find( dataset(:,3) == i );
end
idx_myo = find( dataset(:, 6) == 1 );
idx_pho = find( dataset(:, 6) == 0 );

% add average unlock time at phone (still to be measured).
unlockTime = 5;
dataset(idx_pho, 5) = dataset(idx_pho, 5) + unlockTime;

% add 2 clicks for unlock/lock at phone.
dataset(idx_pho, 4) = dataset(idx_pho, 4) + 2;


%% CHECK DATA INTEGRITY
% this part of the code gives a matrix ('missing) containing the missing 
% how many taskID are missing per user.
% The first column is the userID, the second column is the number of
% missing taskID for the myo, the third column is the number of missing
% taskID for the phone.

compare = [1:20]';
for i = 1:totUsers
    temp = dataset( intersect( idx_user{i}, idx_myo), 2);
    missing_myo{i} = setdiff( compare, temp);
    temp2 = dataset( intersect( idx_user{i}, idx_pho), 2);
    missing_pho{i} = setdiff( compare, temp2);
end


%% MYO SINGLE TASK DIFFICULTY in terms of time-to-complete-task

% in this section we calculate the time necessary to complete each taskLab 
% averaging among users and rounds. The purpose is to check if all tasks 
% have the same difficulty level.

% here we distinguish lines by taskLab, userID, myo.
% To select myo data of userID=1 and taskID=2 use idx_mix{1,2}
% Rows correspond to userID and columns to taskID.
clearvars idx_mix
for i = 1:totUsers
    for j = 1:20
        idx_mix{i,j} = intersect( intersect( idx_myo, idx_user{i}), idx_taskID{j} );
    end
end

% Initialize a matrix of totUsers rows and 5 columns containing for each
% userID the average time for completed taskLab => element (3,2) in the
% matrix corresponds to the time for userID=3 to complete taskLab=2,
% averaged over all 4 rounds for that taskLab.
% In the end we compute the average time for each taskLab among all users
% (by summing elements on each column).

MYO_taskDifficultyTime = zeros( totUsers, 5, 5);

for i = 1:totUsers
    % initialize a matrix of zeros: for each taskLab of the userID it
    % will be incremented by 1 in the corresponding element taskLab. 
    % Useful to understand if any task is missing in some round and
    % will be used when computing the average time among rounds for a
    % particular taskLab.
    roundTask = zeros(1,5);
    for j = 1:20
        % eventually missing tasks are neglected:
        % for taskID=i we count how many users are missing it and we remove
        % them from the computation of the average time
        if nnz( missing_myo{i} == j) ~= 0
            continue
        end
        % check which taskLab the taskID corresponds to and insert it in
        % the corresponding element in the matrix
        taskLab = unique( dataset( idx_mix{i,j}, 3));
        roundTask( taskLab) = roundTask( taskLab) + 1;
        MYO_taskDifficultyTime( i, taskLab, roundTask( taskLab)) = ...
            max( dataset( idx_mix{i,j}, 5));
    end
    % clear roundTask for next userID
    clearvars roundTask
end

% average over rounds for each userID and each taskLab
for i = 1:totUsers
    for j = 1:5
        MYO_taskDifficultyTime( i,j,1) = ...
            mean( nonzeros( MYO_taskDifficultyTime( i,j,:)));
    end
end

% average over userID
MYO_taskDifficultyTime( 1,:,1) = mean( MYO_taskDifficultyTime(:,:,1));

% plot
figure('Name', 'Myo task label time')
plot( MYO_taskDifficultyTime( 1, :, 1), '-ob', 'MarkerFaceColor', 'b')

% plot settings
title( 'MYO task label difficulty (time per task)')
xlabel( 'task label')
ylabel( '< time >' )
set( gca, 'XTick', [1:5])

% use difficulty as a weigth for task time when plotting learning curve
weights = MYO_taskDifficultyTime(1,1,1) * ...
    (ones( 1,5) ./ MYO_taskDifficultyTime( 1,:,1));





%% MYO LEARNING CURVE AMONG ALL 20 TASKS (SISTEMARE DETTAGLI GRAFICI)

% here we distinguish lines by userID, taskID, myo.
% To select myo data of userID=1 and taskID=2 use idx_mix{1,2}.
% Rows correspond to userID and columns to taskID.
for i = 1:totUsers
    for j = 1:20
        idx_mix{i,j} = intersect( intersect( idx_myo, idx_user{i}), idx_taskID{j} );
    end
end

% Initialize a matrix of totUsers rows by 20 columns that will contain 
% the time necessary for each userID to complete taskID
% The time of each single line is cumulative until the taskID increment.
% Total time for the taskID to be completed is the maximum of all the time
% for that taskID.
MYO_taskTime = zeros( totUsers, 20, 2);
for i = 1:20
    for j = 1:totUsers
        % eventually missing tasks are neglected:
        % for taskID=i we count how many users are missing it and we remove
        % them from the computation of the average time
        if nnz( missing_myo{j} == i) ~= 0
            continue
        end
        % time for completed taskID is the max for that particular taskID
        MYO_taskTime(j,i,1) = max( dataset( idx_mix{j,i}, 5));
        MYO_taskTime(j,i,2) = unique( dataset( idx_mix{j,i}, 3));
    end
end

% average time for each taskID
MYO_averageTaskTime = zeros( 1, 20);
for i = 1:20
    MYO_averageTaskTime(i) = mean( nonzeros( MYO_taskTime( :,i,1)));
end

% weighted average time for each taskID
MYO_weightedAverageTaskTime = zeros( 1, 20);
for i = 1:20
    missing = 0;
    for j = 1:totUsers
        if nnz( missing_myo{j} == i) ~= 0
            missing = missing + 1;
            continue
        end
        MYO_weightedAverageTaskTime(1, i) = MYO_weightedAverageTaskTime(1, i) + ...
            weights( MYO_taskTime(j,i,2)) .* MYO_taskTime(j,i,1);
    end
    MYO_weightedAverageTaskTime(1,i) = MYO_weightedAverageTaskTime(1,i) / ( sum( weights) * (totUsers - missing) );
end

% minimum time for each taskID
temp = MYO_taskTime;
temp( ~temp) = Inf;
MYO_minTaskTime = min( temp);
clearvars temp

% maximum time for each taskID
MYO_maxTaskTime = max( MYO_taskTime);

% plot average time with min and max times
fig1 = figure('Name', 'Learning curve: average, min, max')
plot( MYO_averageTaskTime(:))
hold on
plot( MYO_minTaskTime(:))
hold on
plot( MYO_maxTaskTime(:))

% plot percentiles of task time with boxplot
fig2 = figure('Name', 'Learning curve: percentiles (25, 75)')
boxplot( MYO_taskTime, 'Whisker', 0)
h=findobj(gca,'tag','Outliers');
delete(h)
set( gca, 'YLim', [0,100])
hold on
plot( MYO_averageTaskTime, '-or', 'MarkerFaceColor', 'r')
hold on

% compute percentiles (25,75) and plot them
perc25_75 = prctile( MYO_taskTime, [25,75]);
errBar = perc25_75(1,:) - MYO_averageTaskTime;
errBar = [ errBar; perc25_75(2,:) - MYO_averageTaskTime];
fig3 = figure('Name', 'Learning curve: percentiles (25, 75)')
plot( MYO_averageTaskTime, '-ob', 'MarkerFaceColor', 'b')
hold on
errorbar([1:20], MYO_averageTaskTime, errBar(1,:), errBar(2,:), ...
    'r', 'Marker', 'none', 'LineStyle', 'none', 'LineWidth', 1.2 );

% plot percentiles (25,75) as curves
fig4 = figure('Name', 'Learning curve: percentiles curves (25, 75)')
plot( MYO_averageTaskTime, '-ob', 'MarkerFaceColor', 'b')
hold on
plot( perc25_75(1,:), '-^r')
hold on
plot( perc25_75(2,:), '-^r')

% plot settings
set( gca, 'xTick', [1:20])
set( gca, 'xLim', [0,21])


fig5 = figure('Name', 'Power law')
plot( MYO_averageTaskTime)
hold on
pLaw = [1:20] .^ (-.4) .* MYO_averageTaskTime(1);
plot( pLaw);



% plot settings
title( 'MYO and PHONE learning curve')
xlabel( 'task #')
ylabel( '< time >' )


%% PHONE LEARNING CURVE AMONG ALL 20 TASKS (SISTEMARE COME PER MYO)

% here we distinguish lines by userID, taskID, pho.
% To select myo data of userID=1 and taskID=2 use idx_mix{1,2}.
% Rows correspond to userID and columns to taskID.
for i = 1:totUsers
    for j = 1:20
        idx_mix{i,j} = intersect( intersect( idx_pho, idx_user{i}), idx_taskID{j} );
    end
end

% Initialize an array of 20 elements that will contain the time necessary 
% for each single completed taskID, for each single userID.
% The time of each single line is cumulative until the taskID increment.
% Total time for the taskID to be completed is the maximum of all the time
% for that taskID.
PHO_timeCompletedTask = zeros(20, 2);
for i = 1:20
    missing = 0;
    for j = 1:totUsers
        % eventually missing tasks are neglected:
        % for taskID=i we count how many users are missing it and we remove
        % them from the computation of the average time
        if nnz( missing_pho{j} == i) ~= 0
            missing = missing + 1;
            continue
        end
        % time for completed taskID is averaged over users
        PHO_timeCompletedTask(i,1) = PHO_timeCompletedTask(i,1) + max( dataset( idx_mix{j,i}, 5));
        % compute variance (for error bars)
        PHO_timeCompletedTask(i,2) = PHO_timeCompletedTask(i,2) + ( max( dataset( idx_mix{j,i}, 5)))^2
    end
    PHO_timeCompletedTask(i,1) = PHO_timeCompletedTask(i,1) / (totUsers - missing);
    PHO_timeCompletedTask(i,2) = PHO_timeCompletedTask(i,2) / (totUsers - missing);
    PHO_timeCompletedTask(i,2) = sqrt( PHO_timeCompletedTask(i,2) - ( PHO_timeCompletedTask(i,1) )^2 );
end

hold on
plot( PHO_timeCompletedTask, '-^r', 'MarkerFaceColor', 'r' )


% plot settings
legend( 'MYO','PHONE')



%% MYO SINGLE TASK DIFFICULTY in terms of clicks-to-complete-task (TEMP)

% in this section we calculate the number of 'clicks' necessary to 
% complete each taskLab averaging among users and rounds. The purpose is 
% to check if all tasks have the same difficulty level.

% here we distinguish lines by taskLab, userID, myo.
% To select myo data of userID=1 and taskID=2 use idx_mix{1,2}
% Rows correspond to userID and columns to taskID.
clearvars idx_mix

for i = 1:totUsers
    for j = 1:20
        idx_mix{i,j} = intersect( intersect( idx_myo, idx_user{i}), idx_taskID{j} );
    end
end

% Initialize a matrix of totUsers rows and 5 columns containing for each
% userID the average number of clicks for completed taskLab => element 
% (3,2) in the matrix corresponds to the number of clicks for userID=3 
% to complete taskLab=2, averaged over all 4 rounds for that taskLab.
% NOTE: number of clicks is not cumulative during taskID => clicks will be
% summed over that taskID.
% In the end we compute the average number of clicks for each taskLab 
% among all users (by summing elements on each column).

MYO_taskDifficultyClicks = zeros( totUsers, 5);

for i = 1:totUsers
    % initialize a matrix of zeros: for each taskLab of the userID it
    % will be incremented by 1 in the corresponding element taskLab. 
    % Useful to understand if any task is missing in some round and
    % will be used when computing the average number of clicks among 
    % rounds for a particular taskLab.
    roundTask = zeros(1,5);
    for j = 1:20
        % eventually missing tasks are neglected:
        % for taskID=i we count how many users are missing it and we remove
        % them from the computation of the average number of clicks.
        if nnz( missing_myo{i} == j) ~= 0
            continue
        end
        % check which taskLab the taskID corresponds to and insert it in
        % the corresponding element in the matrix
        taskLab = unique( dataset( idx_mix{i,j}, 3));
        MYO_taskDifficultyClicks( i, taskLab) = MYO_taskDifficultyClicks( i, taskLab) + sum( dataset( idx_mix{i,j}, 4));
        roundTask( 1, taskLab) = roundTask( 1, taskLab) + 1;
    end
    % average among all rounds (some taskLab may miss a round!)
    MYO_taskDifficultyClicks( i, :) ./ roundTask;
    clearvars roundTask
end


% here we calculate average taskLab number of clicks among all users and 
% we write it in the first line of MYO_taskDifficultyClicks
for i = 1:5
    MYO_taskDifficultyClicks( 1, i) = mean( MYO_taskDifficultyClicks( :, i));
end

figure(3)
plot( MYO_taskDifficultyClicks( 1, :), '-or', 'MarkerFaceColor', 'r')

% plot settings
title( 'MYO single task difficulty (number of clicks for task)')
xlabel( 'task label')
ylabel( '< clicks >' )
set( gca, 'XTick', [1:5])



%% MYO SINGLE TASK DIFFICULTY in terms of attempts-to-complete-task (TEMP)

% in this section we calculate the number of attempts necessary to 
% complete each taskLab averaging among users and rounds. The purpose is 
% to check if all tasks have the same difficulty level.

% here we distinguish lines by taskLab, userID, myo.
% To select myo data of userID=1 and taskID=2 use idx_mix{1,2}
% Rows correspond to userID and columns to taskID.
clearvars idx_mix

for i = 1:totUsers
    for j = 1:20
        idx_mix{i,j} = intersect( intersect( idx_myo, idx_user{i}), idx_taskID{j} );
    end
end

% Initialize a matrix of totUsers rows and 5 columns containing for each
% userID the average number of attempts for completed taskLab => element 
% (3,2) in the matrix corresponds to the number of attempts for userID=3 
% to complete taskLab=2, averaged over all 4 rounds for that taskLab.
% NOTE: number of attempts is the number of consecutive rows with same
% taskID and taskLab.
% In the end we compute the average number of attempts for each taskLab 
% among all users (by summing elements on each column).

MYO_taskDifficultyAttempts = zeros( totUsers, 5);

for i = 1:totUsers
    % initialize a matrix of zeros: for each taskLab of the userID it
    % will be incremented by 1 in the corresponding element taskLab. 
    % Useful to understand if any task is missing in some round and
    % will be used when computing the average number of clicks among 
    % rounds for a particular taskLab.
    roundTask = zeros(1,5);
    for j = 1:20
        % eventually missing tasks are neglected:
        % for taskID=i we count how many users are missing it and we remove
        % them from the computation of the average number of clicks.
        if nnz( missing_myo{i} == j) ~= 0
            continue
        end
        % count number of attempts for each task (count number of rows with
        % same consecutive taskID j
        attempts = length( idx_mix{i,j})
        % check which taskLab the taskID corresponds to and insert it in
        % the corresponding element in the matrix
        taskLab = unique( dataset( idx_mix{i,j}, 3));
        MYO_taskDifficultyAttempts( i, taskLab) = MYO_taskDifficultyAttempts( i, taskLab) + attempts;
        roundTask( 1, taskLab) = roundTask( 1, taskLab) + 1;
    end
    % average among all rounds (some taskLab may miss a round!)
    MYO_taskDifficultyAttempts( i, :) ./ roundTask;
    clearvars roundTask
end


% here we calculate average taskLab number of attempts among all users and 
% we write it in the first line of MYO_taskDifficultyAttempts
for i = 1:5
    MYO_taskDifficultyAttempts( 1, i) = mean( MYO_taskDifficultyAttempts( :, i));
end

figure(4)
plot( MYO_taskDifficultyAttempts( 1, :), '-or', 'MarkerFaceColor', 'r')

% plot settings
title( 'MYO single task difficulty (number of attempts for task)')
xlabel( 'task label')
ylabel( '< attempts >' )
set( gca, 'XTick', [1:5])


