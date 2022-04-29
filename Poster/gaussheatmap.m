% Peter Ha
% Random heatmap generator with n hot and cold spots for visualization

clear
close all

N = 1000; % number of iterations
n = 5; % number of hotspots/traps
% no it isn't confusing actually 

layer = 256;
layers = zeros(layer);

canvas = 1024;
canvass = 0.5*ones(canvas, canvas, N);

spots = 1024*rand([2*n,2]); % [hotspot x, hotspot y; coldspot x coldspot y; ...]
hotspots = round(spots(1:2:end,:));

randspots = round(1024*rand([n,2])); % random spots for trad trapping

hauls = zeros(N, 2); % [trad, data; ...]

for j = 1:N
    for i = 1:n
        canvass(:,:,j) = canvass(:,:,j)+0.5*gauss2d(canvass(:,:,j), 500*rand(1), spots(2*i-1,:));
        canvass(:,:,j) = canvass(:,:,j)-0.5*gauss2d(canvass(:,:,j), 500*rand(1), spots(2*i,:));
        hauls(j,1) = hauls(j,1) + canvass(randspots(i,1),randspots(i,2),j);
    end
    for i = 1:n
        hauls(j,2) = hauls(j,2) + canvass(hotspots(i,1),hotspots(i,2),j);
    end
end

nethauls = [sum(hauls(:,1)), sum(hauls(:,2))];
ratio = nethauls(2)/nethauls(1)
    
heatmap(canvass(:,:,j))
grid off

function mat = gauss2d(mat, sigma, center)
    gsize = size(mat);
    [R,C] = ndgrid(1:gsize(1), 1:gsize(2));
    mat = gaussC(R,C, sigma, center);
end

function val = gaussC(x, y, sigma, center)
    xc = center(1);
    yc = center(2);
    exponent = ((x-xc).^2 + (y-yc).^2)./(2*sigma^2);
    val       = (exp(-exponent));
end