#include <defuse.hpp>
#include <cplusutil.hpp>
#include <string>

int main(int argc, char const *argv[])
{

  std::string briskDataRoot = "./";

  File *f = new File(briskDataRoot + "brisk_data/smoke.mp4");
  defuse::VideoBase *vb = new defuse::VideoBase(f);
  defuse::BRISKXtractor *be = new defuse::BRISKXtractor();

  File *f2 = new File(briskDataRoot + "brisk_data/no_smoke.mp4");
  defuse::VideoBase *vb2 = new defuse::VideoBase(f2);
  defuse::BRISKXtractor *be2 = new defuse::BRISKXtractor();

  // distance
  defuse::Hamming *ham = new defuse::Hamming();

  // SIGXtractor *be = new SIGXtractor(vb);
  // be->testBrisk(vb, 10);
  defuse::FeaturesBase *fb;
  defuse::FeaturesBase *fb2;
  fb = be->xtract(vb);
  fb2 = be2->xtract(vb2);
  LOG_INFO("1 File: " << f->getFile());
  LOG_INFO("1 Descriptor Length: " << fb->mVectors.rows << " x " << fb->mVectors.cols);
  LOG_INFO("1 Brisk Extract Time: " << fb->mExtractionTime);

  LOG_INFO("2 File: " << f2->getFile());
  LOG_INFO("2 Descriptor Length: " << fb2->mVectors.rows << " x " << fb2->mVectors.cols);
  LOG_INFO("2 Brisk Extract Time: " << fb2->mExtractionTime);

  // LOG_INFO("Mat object1" << fb->mVectors);
  // LOG_INFO("Type: " << fb->mVectors.type());

  double e1StartNorm = double(cv::getTickCount());
  float f1_f2_norm = ham->computeNorm(fb->mVectors, fb2->mVectors);
  double e1EndNorm = double(cv::getTickCount());
  double elapsedSecsNorm = (e1EndNorm - e1StartNorm) / double(cv::getTickFrequency());

  LOG_INFO("Hamming Norm File 1 - File 1: " << ham->computeNorm(fb->mVectors, fb->mVectors))
  LOG_INFO("Hamming Norm File 1 - File 2: " << f1_f2_norm << " (runtime: "<< elapsedSecsNorm << ")")

  double e1StartBF = double(cv::getTickCount());
  float f1_f2_bf = ham->compute(*fb, *fb2);
  double e1EndBF = double(cv::getTickCount());
  double elapsedSecsBF = (e1EndBF - e1StartBF) / double(cv::getTickFrequency());

  LOG_INFO("Hamming BF File 1 - File 1: " << ham->compute(*fb, *fb))
  LOG_INFO("Hamming BF File 1 - File 2: " << f1_f2_bf << " (runtime: "<< elapsedSecsBF << ")")


  return 0;
}
