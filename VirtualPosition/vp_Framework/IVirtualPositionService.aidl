package android.os;

interface IVirtualPositionService {
    void setVirtualToggle(int tog);
    int getVirtualToggle();
    void setVirtualLatitude(double vlat);
    double getVirtualLatitude();
    void setVirtualLongitude(double vlon);
    double getVirtualLongitude();
}
