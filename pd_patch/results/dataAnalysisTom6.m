%% DATA ANALYSIS Tom

clc
clear all
close all

%% read the data file

dp='C:\Users\Thomas\Documents\Github\MYO-SMC705\pd_patch\results\';
% 1 1 1 4 5 14.0052 myo; './tasks_CLEANED_removed8.txt'
% fid=fopen([dp 'tasks_CLEANED_Tom.txt']) ; % Opens target file
fid=fopen('./tasks_CLEANED_Tom.txt') ; % Opens target file (alt. path)

data=textscan(fid ,'%f%f%f%f%f%f%s','delimiter',' '); % Reads data as floats and strings
fclose(fid);

userID = [ data{1,2} ];
taskID = [ data{1,3} ];
taskLab = [ data{1,4} ];
clicks = [ data{1,5} ];
taskTime=[data{1,6}];
device=[data{1,7}];
device=1*strcmp(device, 'myo;')+2*strcmp(device,'pho;');

%% Create Matrix

allData = [userID taskID taskLab clicks taskTime device]; 

%% Separate Phone and MYO data indices

%Get Indices for Myo and Phone.
% Note this method will eliminate any data that is not labelled either 'myo;' or 'pho;' 
idx_allMYO=find(device==1);
idx_allPho=find(device==2);

%% Create MYO and Phone matrices

%MYO Matrix
userID_MYO = userID(idx_allMYO,:);
taskID_MYO = taskID(idx_allMYO,:);
taskLab_MYO = taskLab(idx_allMYO,:);
clicks_MYO = clicks(idx_allMYO,:);
taskTime_MYO=taskTime(idx_allMYO,:);
allData_MYO=[userID_MYO taskID_MYO taskLab_MYO clicks_MYO taskTime_MYO] ; 

%Phone Matrix
userID_Pho = userID(idx_allPho,:);
taskID_Pho = taskID(idx_allPho,:);
taskLab_Pho = taskLab(idx_allPho,:);
clicks_Pho = clicks(idx_allPho,:);
taskTime_Pho=taskTime(idx_allPho,:);
allData_Pho=[userID_Pho taskID_Pho taskLab_Pho clicks_Pho taskTime_Pho] ; 

%Check:
% size(allData_MYO) + size(allData_Pho) == size(allData) . If this does not
% add up then there are lines of errors in the coding.


%% Separate by userID for MYO and Phone

Num_Users =max(userID); %Maximum number of users

%% Cell containing matrices for each user's results with the MYO

for i = 1:Num_Users
    Individual_User_MYO{i} = allData_MYO( find( userID_MYO==i), :);
end

%% Cell containing matrices for each user's results with the Phone

for i = 1:Num_Users
    Individual_User_Pho{i} = allData_Pho( find( userID_Pho==i), :);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% MYO-Scatter Plot Maximum Task Time Vs Task Iteration for task label
% (Separated into figures for each Task Label and Coloured based on UserID ) 
%idx_monster=7
C=(colormap(lines));
for idx_monster=1:Num_Users
clear('AnnataskID')
clear('idx_Tasktime_max') 
clear('Anna_results_MYO')
clear('AnnaXbob')
clear('AnnaYbob')
clear('AnnaZbob')
clear('Annabob')
clear('idx_TID')
clear('AnnataskID')
clear('idx_TM')
clear(' idx_Tasktime_max')
clear('X_nu')
clear('Y_nu')
clear('Z_nu')
clear('Anna3D_max')
clear('idx_taskLab')
clear('idx_sorted')
clear('idx_taskLab2')
clear('taskmax_divided')
clear('idx_taskLab3')
clear('non_iter')
clear('X')
clear('Anna3D_sorted')

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Anna_results_MYO=(Individual_User_MYO{idx_monster}); %Change _MYO to _Pho to plot same results for the phone
AnnaXbob=Anna_results_MYO(:,2);
AnnaYbob=Anna_results_MYO(:,3);
AnnaZbob=Anna_results_MYO(:,5);

Annabob=[AnnaXbob AnnaYbob AnnaZbob];

%Tasktimes under same TaskID
for idx_TID=1:max(AnnaXbob); %TaskID 
        AnnataskID{idx_TID}=find(AnnaXbob==idx_TID);
end

%Max Timetask for each TaskID
for idx_TM=1:length(AnnataskID); %TaskMax
    idx_Tasktime_max(idx_TM)=max(AnnataskID{idx_TM});
end

%Matrix for tasktime maximums
X_nu=(AnnaXbob(idx_Tasktime_max));
Y_nu=(AnnaYbob(idx_Tasktime_max));
Z_nu=(AnnaZbob(idx_Tasktime_max));
Anna3D_max=[X_nu Y_nu Z_nu];

%Indices for tasklabels
for idx_taskLab=1:5;
        idx_sorted{idx_taskLab}=find(Y_nu==idx_taskLab); 
end

%applied indices for task labels
for idx_taskLab=1:5;
taskmax_divided{idx_taskLab}=(Anna3D_max(idx_sorted{idx_taskLab},:));
end

%for idx_taskLab=1:5;
%taskall_divided{idx_taskLab}=(Annabob(idx_sorted{idx_taskLab},:));
%end

%Plot Attempts under max task times
%for idx_taskLab=1:5;
%non_iter=[1:length(taskall_divided{1,idx_taskLab})]';
%    figure(idx_taskLab);hold on;
%      X2=taskall_divided{1,idx_taskLab}
%scatter(X(),X2(:,3)); hold on;
%end
%Plot total task times over task ID
for idx_taskLab=1:5
    clear('non_iter')
    clear('X')
    clear('Anna3D_sorted')
    clear('I1')
    [I1 J2]=size(taskmax_divided{1,idx_taskLab});
non_iter=[1:I1]';
    figure(idx_taskLab);hold on;%idx_taskLab
       X=taskmax_divided{1,idx_taskLab}; 
Anna3D_sorted=X;
%plot(non_iter,Anna3D_sorted(:,3),'color',C(idx_monster,:)); hold on;% swap X position with Anna3D_sorted(:,1) for over iter
%scatter(non_iter,Anna3D_sorted(:,3),10,C(idx_monster,:)); hold on
%plot(non_iter,Anna3D_sorted(:,3),'color','k'); hold on;
scatter(non_iter,Anna3D_sorted(:,3),10,'r'); hold on
end
end

%% MYO Plot Task time Learning curve for all users on Individual task labels
for idx_figure=1:5
    figure(idx_figure); hold on;
h = gcf ;
axesObjs = get(h, 'Children'); %axes handles
dataObjs = get(axesObjs, 'Children'); %handles to low-level graphics objects in axes
objTypes = get(dataObjs, 'Type');%type of low-level graphics object
xdata = get(dataObjs, 'XData') ; %data from low-level grahics objects
ydata = get(dataObjs, 'YData');
XY_1=[xdata{1}'  ydata{1}'];
XY_2=[xdata{2}'  ydata{2}'];
XY_3=[xdata{3}'  ydata{3}'];
XY_4=[xdata{4}'  ydata{4}'];
XY_5=[xdata{5}'  ydata{5}'];
XY_6=[xdata{6}'  ydata{6}'];
XY_7=[xdata{7}'  ydata{7}'];
XY_8=[xdata{8}'  ydata{8}'];
XY_9=[xdata{9}'  ydata{9}'];
XY_10=[xdata{10}'  ydata{10}'];
XY_11=[xdata{11}'  ydata{11}'];
XY_12=[xdata{12}'  ydata{12}'];
XY_13=[xdata{13}'  ydata{13}'];
XY_14=[xdata{14}'  ydata{14}'];
XY_15=[xdata{15}'  ydata{15}'];
XY_all=[XY_1;XY_2;XY_3;XY_4;XY_5;XY_6;XY_7;XY_8;XY_9;XY_10;XY_11;XY_12;XY_13;XY_14;XY_15];
idx_XY1=find(XY_all(:,1)==1);
idx_XY2=find(XY_all(:,1)==2);
idx_XY3=find(XY_all(:,1)==3);
idx_XY4=find(XY_all(:,1)==4);
idx_XY5=find(XY_all(:,1)==5);
XY_done=[mean(XY_all(idx_XY1,2));mean(XY_all(idx_XY2,2));mean(XY_all(idx_XY3,2));mean(XY_all(idx_XY4,2))];
error_standarddeviation=[std(XY_all(idx_XY1,2));std(XY_all(idx_XY2,2));std(XY_all(idx_XY3,2));std(XY_all(idx_XY4,2))];
plot(XY_done,'color','r'); hold on;
errorbar(XY_done(:,1),error_standarddeviation,'rx');
%axis([1,4,0,100])

end

C=(colormap(lines));
for idx_monster=1:Num_Users
clear('AnnataskID')
clear('idx_Tasktime_max') 
clear('Anna_results_MYO')
clear('AnnaXbob')
clear('AnnaYbob')
clear('AnnaZbob')
clear('Annabob')
clear('idx_TID')
clear('AnnataskID')
clear('idx_TM')
clear(' idx_Tasktime_max')
clear('X_nu')
clear('Y_nu')
clear('Z_nu')
clear('Anna3D_max')
clear('idx_taskLab')
clear('idx_sorted')
clear('idx_taskLab2')
clear('taskmax_divided')
clear('idx_taskLab3')
clear('non_iter')
clear('X')
clear('Anna3D_sorted')

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Anna_results_MYO=(Individual_User_Pho{idx_monster}); %Change _MYO to _Pho to plot same results for the phone
AnnaXbob=Anna_results_MYO(:,2);
AnnaYbob=Anna_results_MYO(:,3);
AnnaZbob=Anna_results_MYO(:,5);

Annabob=[AnnaXbob AnnaYbob AnnaZbob];

%Tasktimes under same TaskID
for idx_TID=1:max(AnnaXbob); %TaskID 
        AnnataskID{idx_TID}=find(AnnaXbob==idx_TID);
end

%Max Timetask for each TaskID
for idx_TM=1:length(AnnataskID); %TaskMax
    idx_Tasktime_max(idx_TM)=max(AnnataskID{idx_TM});
end

%Matrix for tasktime maximums
X_nu=(AnnaXbob(idx_Tasktime_max));
Y_nu=(AnnaYbob(idx_Tasktime_max));
Z_nu=(AnnaZbob(idx_Tasktime_max));
Anna3D_max=[X_nu Y_nu Z_nu];

%Indices for tasklabels
for idx_taskLab=1:5;
        idx_sorted{idx_taskLab}=find(Y_nu==idx_taskLab); 
end

%applied indices for task labels
for idx_taskLab=1:5;
taskmax_divided{idx_taskLab}=(Anna3D_max(idx_sorted{idx_taskLab},:));
end

%for idx_taskLab=1:5;
%taskall_divided{idx_taskLab}=(Annabob(idx_sorted{idx_taskLab},:));
%end

%Plot Attempts under max task times
%for idx_taskLab=1:5;
%non_iter=[1:length(taskall_divided{1,idx_taskLab})]';
%    figure(idx_taskLab);hold on;
%      X2=taskall_divided{1,idx_taskLab}
%scatter(X(),X2(:,3)); hold on;
%end
%Plot total task times over task ID
for idx_taskLab=1:5
    clear('non_iter')
    clear('X')
    clear('Anna3D_sorted')
    clear('I1')
    [I1 J2]=size(taskmax_divided{1,idx_taskLab});
non_iter=[1:I1]';
    figure(idx_taskLab);hold on;%idx_taskLab
       X=taskmax_divided{1,idx_taskLab}; 
Anna3D_sorted=X;
%plot(non_iter,Anna3D_sorted(:,3),'color',C(idx_monster,:)); hold on;% swap X position with Anna3D_sorted(:,1) for over iter
%scatter(non_iter,Anna3D_sorted(:,3),10,C(idx_monster,:)); hold on
%plot(non_iter,Anna3D_sorted(:,3),'color','y'); hold on;
scatter(non_iter,Anna3D_sorted(:,3),10,'b'); hold on
end
end

%% MYO Plot Task time Learning curve for all users on Individual task labels
for idx_figure=1:5
    figure(idx_figure); hold on;
h = gcf ;
axesObjs = get(h, 'Children'); %axes handles
dataObjs = get(axesObjs, 'Children'); %handles to low-level graphics objects in axes
objTypes = get(dataObjs, 'Type');%type of low-level graphics object
xdata = get(dataObjs, 'XData') ; %data from low-level grahics objects
ydata = get(dataObjs, 'YData');
XY_1=[xdata{1}'  ydata{1}'];
XY_2=[xdata{2}'  ydata{2}'];
XY_3=[xdata{3}'  ydata{3}'];
XY_4=[xdata{4}'  ydata{4}'];
XY_5=[xdata{5}'  ydata{5}'];
XY_6=[xdata{6}'  ydata{6}'];
XY_7=[xdata{7}'  ydata{7}'];
XY_8=[xdata{8}'  ydata{8}'];
XY_9=[xdata{9}'  ydata{9}'];
XY_10=[xdata{10}'  ydata{10}'];
XY_11=[xdata{11}'  ydata{11}'];
XY_12=[xdata{12}'  ydata{12}'];
XY_13=[xdata{13}'  ydata{13}'];
XY_14=[xdata{14}'  ydata{14}'];
XY_15=[xdata{15}'  ydata{15}'];
XY_all=[XY_1;XY_2;XY_3;XY_4;XY_5;XY_6;XY_7;XY_8;XY_9;XY_10;XY_11;XY_12;XY_13;XY_14;XY_15];
idx_XY1=find(XY_all(:,1)==1);
idx_XY2=find(XY_all(:,1)==2);
idx_XY3=find(XY_all(:,1)==3);
idx_XY4=find(XY_all(:,1)==4);
idx_XY5=find(XY_all(:,1)==5);
XY_done=[mean(XY_all(idx_XY1,2));mean(XY_all(idx_XY2,2));mean(XY_all(idx_XY3,2));mean(XY_all(idx_XY4,2))];
error_standarddeviation=[std(XY_all(idx_XY1,2));std(XY_all(idx_XY2,2));std(XY_all(idx_XY3,2));std(XY_all(idx_XY4,2))];
plot(XY_done,'color','b'); hold on;
errorbar(XY_done(:,1),error_standarddeviation,'bx');
axis([0.9,4.1,0,85])
grid minor
end


%% RUN UP TO HERE


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%%
%%
%%
%%
%%
%%
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
