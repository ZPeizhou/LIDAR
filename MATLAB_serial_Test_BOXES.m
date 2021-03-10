s = serial('/dev/cu.usbmodem14201');
set (s,'BaudRate',9600);
fopen(s);

datanumber = 5120;
t = 1;
data = [];
r = 0;
PointCloudData = [];
while (t < datanumber)
    data(t,1) = str2num(fgetl(s));
    data(t,2) = 70 + str2num(fgetl(s));
    data(t,3) = -45 + str2num(fgetl(s));
    r = data(t,1);
    
    if (r < 60)&&(r > 0)
        PointCloudData(t,1) = r*sin(data(t,2)*pi/180)*cos(data(t,3)*pi/180)
        PointCloudData(t,2) = r*sin(data(t,2)*pi/180)*sin(data(t,3)*pi/180)
        PointCloudData(t,3) = r*cos(data(t,2)*pi/180)
        t
        x = PointCloudData(t,1);
        y = PointCloudData(t,2);
        z = PointCloudData(t,3);
        if (r < 20 )
            hold on
            plot3(x,y,z,'o','Color','k')
            grid on
            view([-81 19])
            drawnow
        end
        if (r >= 20)&&(r < 30)
            hold on
            plot3(x,y,z,'o','Color','b')
            grid on
            view([-81 19])
            drawnow
        end
        if (r >= 30)&&(r < 35)
            hold on
            plot3(x,y,z,'o','Color','g')
            grid on
            view([-81 19])
            drawnow
        end
        if (r >= 35)&&(r < 45)
            hold on
            plot3(x,y,z,'o','Color','c')
            grid on
            view([-81 19])
            drawnow
        end
        if (r >= 45)
            hold on
            plot3(x,y,z,'o','Color','y')
            grid on
            view([-81 19])
            drawnow
        end
    end
    if (r >= 60)||(r < 0)
        PointCloudData(t,1) = PointCloudData(t-1,1)
        PointCloudData(t,2) = PointCloudData(t-1,2)
        PointCloudData(t,3) = PointCloudData(t-1,3)
        t
        x = PointCloudData(t,1);
        y = PointCloudData(t,2);
        z = PointCloudData(t,3);
        hold on
        plot3(x,y,z,'o','Color','r')
        grid on
        view([-81 19])
        drawnow
    end
    t = t+1;
end
fclose(s);