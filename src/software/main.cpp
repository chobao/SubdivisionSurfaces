#include "Visualization/GLViewer.h"

int main() {
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    viewer.SetUp(800,600);
    viewer.Run();
}