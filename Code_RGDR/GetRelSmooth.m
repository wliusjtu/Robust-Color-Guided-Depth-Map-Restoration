function [RS] = GetRelSmooth(Depth, winR, patchR)

[m, n] = size(Depth);

Depth = padarray(Depth, [winR, winR], 'symmetric');

% ====================== min operation =============================
Temp = Depth(1:2*winR + 1, :);

[Seed, SeedX] = min(Temp, [], 1);
SeedX = abs(winR - SeedX) + 1; % substract the patchR caused by padarray
SeedY_ = 1:n;
Seed = Seed(winR + 1: end - winR);
SeedX = SeedX(winR + 1: end - winR);


% tic
[MinMapV, MinCoorX, ~, ~] = mexConstantMinMax(Depth, Seed, SeedX, SeedY_, winR, 0, 0);
% toc



MinMapV = MinMapV';
MinMapV = padarray(MinMapV, [winR, winR], 'symmetric');

Temp = MinMapV(1:2*winR + 1, :);

[Seed, SeedY] = min(Temp, [], 1);
SeedY = abs(winR - SeedY) + 1;
SeedX_ = 1:m;
Seed = Seed(winR + 1: end - winR);
SeedY = SeedY(winR + 1: end - winR);

[MinMap, MinCoorY, ~, ~] = mexConstantMinMax(MinMapV, Seed, SeedY, SeedX_, winR, 0, 0);

MinMap = MinMap';
MinCoorY = MinCoorY';

% ====================== max operation =============================
Temp = Depth(1:2*winR + 1, :);

[Seed, SeedX] = max(Temp, [], 1);
SeedX = abs(winR - SeedX) + 1; % substract the patchR caused by padarray
SeedY_ = 1:n;
Seed = Seed(winR + 1: end - winR);
SeedX = SeedX(winR + 1: end - winR);

[MaxMapV, MaxCoorX, ~, ~] = mexConstantMinMax(Depth, Seed, SeedX, SeedY_, winR, 1, 0);

MaxMapV = MaxMapV';
MaxMapV = padarray(MaxMapV, [winR, winR], 'symmetric');

Temp = MaxMapV(1:2*winR + 1, :);

[Seed, SeedY] = max(Temp, [], 1);
Seed = Seed(winR + 1: end - winR);
SeedY = abs(winR - SeedY) + 1;
SeedY = SeedY(winR + 1: end - winR);
SeedX_ = 1:m;


[MaxMap, MaxCoorY, ~, ~] = mexConstantMinMax(MaxMapV, Seed, SeedY, SeedX_, winR, 1, 0);

MaxMap = MaxMap';
MaxCoorY = MaxCoorY';

Depth = Depth(winR + 1: end - winR, winR + 1: end - winR);

% tic
RS = mexGetRelSmooth(Depth, MinCoorX, MinCoorY, MaxCoorX, MaxCoorY, patchR);
% toc