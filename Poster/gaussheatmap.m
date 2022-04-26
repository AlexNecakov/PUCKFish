% Peter Ha
% Random heatmap generator with n hot and cold spots for visualization

clear
close all

n = 5;

layer = 256;
layers = zeros(layer);

canvas = 1024;
canvass = 0.5*ones(canvas);

for i = 1:n
    canvass = canvass+0.5*gauss2d(canvass, 500*rand(1), 1024*rand([1,2]));
    canvass = canvass-0.5*gauss2d(canvass, 500*rand(1), 1024*rand([1,2]));
end
    
heatmap(canvass)
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