% function DepthMapExtract

mkdir('.\input')

Name = {'art', 'book', 'dolls', 'laundry', 'moebius', 'reindeer'};

for i = 1: 6
    
    Color = imread([Name{i}, '_color.png']);
    Depth = imread([Name{i}, '_big.png']);
    imwrite(Color, ['.\input\',Name{i}, '_color.png']);
    imwrite(Depth, ['.\input\',Name{i}, '_clean.png']);
    
    DepthSmall = imread(['.\',Name{i},'_big\depth_1_n.png']);
    imwrite(DepthSmall, ['.\input\',Name{i}, '_small.png']);
    
    DepthSmall = imread(['.\',Name{i},'_big\depth_2_n.png']);
    imwrite(DepthSmall, ['.\input\',Name{i}, '_tiny.png']);
    
end