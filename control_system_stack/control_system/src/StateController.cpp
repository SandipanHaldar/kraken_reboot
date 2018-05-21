#include "control_system/StateController.h"

namespace kraken_controller{
    StateController::StateController(){
        for(int i = 0; i<18; i++){
            _pose_error[i] = 0;
            _vel_error[i] = 0;
        }
    }

    StateController::~StateController(){}

    void StateController::poseParam(){
        ROS_INFO("POSITION CONTROL");
        _gain_file = "poseParam";
    }

    void StateController::twistParam(){
        ROS_INFO("TWIST CONTROL");
        _gain_file = "twistParam";
    }

    void StateController::stop(){
        ROS_INFO("STOP");
        _gain_file = "stop";
    }

    void StateController::updateState(const nav_msgs::Odometry &msg){
        // _feedback.pose.position.x = msg->pose.position.x;
        // _feedback.pose.position.y = msg->pose.position.y;
        // _feedback.pose.position.z = msg->pose.position.z;
        // _feedback.pose.orientation.x = msg->pose.orientation.x;
        // _feedback.pose.orientation.y = msg->pose.orientation.y;
        // _feedback.pose.orientation.z = msg->pose.orientation.z;
        // _feedback.pose.orientation.w = msg->pose.orientation.w;
        //
        // _feedback.twist.linear.x = msg->twist.linear.x;
        // _feedback.twist.linear.y = msg->twist.linear.y;
        // _feedback.twist.linear.z = msg->twist.linear.z;
        // _feedback.twist.angular.x = msg->twist.angular.x;
        // _feedback.twist.angular.y = msg->twist.angular.y;
        // _feedback.twist.angular.z = msg->twist.angular.z;
    }

    void StateController::updatePID(geometry_msgs::Pose transPose, geometry_msgs::Twist transTwist){
        _pose_error[3] = _pose_error[0] - transPose.position.x;
        _pose_error[4] = _pose_error[1] - transPose.position.y;
        _pose_error[5] = _pose_error[2] - transPose.position.z;
        _pose_error[6] = _pose_error[6] + transPose.position.x;
        _pose_error[7] = _pose_error[7] + transPose.position.y;
        _pose_error[8] = _pose_error[8] + transPose.position.z;
        _pose_error[0] = transPose.position.x;
        _pose_error[1] = transPose.position.y;
        _pose_error[2] = transPose.position.z;

        double roll, pitch, yaw;
        tf::Quaternion q(transPose.orientation.x, transPose.orientation.y, transPose.orientation.z, transPose.orientation.w);
        //std::cout<<transPose.position.x<<"x\n"<<transPose.position.y<<"y\n"<<transPose.position.z<<"z\n";
        //printf("X%f Y%f Z%f W%f \n", transPose.orientation.x, transPose.orientation.y, transPose.orientation.z, transPose.orientation.w);
        tf::Matrix3x3 temp(q);
        temp.getRPY(roll, pitch, yaw);

        _pose_error[12] = _pose_error[9] - roll;
        _pose_error[13] = _pose_error[10] - pitch;
        _pose_error[14] = _pose_error[11] - yaw;
        _pose_error[15] = _pose_error[15] + roll;
        _pose_error[16] = _pose_error[16] + pitch;
        _pose_error[17] = _pose_error[17] + yaw;
        _pose_error[9] = roll;
        _pose_error[10] = pitch;
        _pose_error[11] = yaw;

<<<<<<< HEAD
        // for(int i = 9; i<12; i++){
        //     std::cout<<i<<_pose_error[i]<<"\n";
        // }
=======
        // for(int i = 0; i<3; i++){
        //     std::cout<<i<<"---"<<_pose_error[i]<<"\n";
        //  }
>>>>>>> upstream/testing-controls
        _vel_error[3] = _vel_error[0] - transTwist.linear.x;
        _vel_error[4] = _vel_error[1] - transTwist.linear.y;
        _vel_error[5] = _vel_error[2] - transTwist.linear.z;
        _vel_error[6] = _vel_error[6] + transTwist.linear.x;
        _vel_error[7] = _vel_error[7] + transTwist.linear.y;
        _vel_error[8] = _vel_error[8] + transTwist.linear.z;
        _vel_error[0] = transTwist.linear.x;
        _vel_error[1] = transTwist.linear.y;
        _vel_error[2] = transTwist.linear.z;

        _vel_error[12] = _vel_error[9] - transTwist.angular.x;
        _vel_error[13] = _vel_error[10] - transTwist.angular.y;
        _vel_error[14] = _vel_error[11] - transTwist.angular.z;
        _vel_error[15] = _vel_error[15] + transTwist.angular.x;
        _vel_error[16] = _vel_error[16] + transTwist.angular.y;
        _vel_error[17] = _vel_error[17] + transTwist.angular.z;
        _vel_error[9] = transTwist.angular.x;
        _vel_error[10] = transTwist.angular.y;
        _vel_error[11] = transTwist.angular.z;
        //  for(int i=0; i<18; i++){
        //      printf("%d %f \n",i, _pose_error[i]);
        // }
        kraken_msgs::PIDError error;
        error.linear.header.frame_id = "/base_link";
        error.linear.header.stamp = ros::Time();
        error.linear.point.x = _pose_error[0];
        error.linear.point.y = _pose_error[1];
        error.linear.point.z = _pose_error[2];

        error.angular.header.frame_id = "/base_link";
        error.angular.header.stamp = ros::Time();
        error.angular.point.x = _pose_error[9];
        error.angular.point.y = _pose_error[10];
        error.angular.point.z = _pose_error[11];

        _PidErrorPub.publish(error);
    }

    void StateController::loadParams(const std::vector<std::string> &filenames){
        for(int i = 0; i<filenames.size(); i++){
            ControlParameters *param = new ControlParameters();
            param->load(filenames[i]);
            _controlParams.push_back(param);
            _controlParams_index[param->getName()] = i;
            std::cout<<_controlParams_index[param->getName()]<<"----"<<filenames[i]<<"\n";
            param->write(std::cerr);
        }
    }

    void StateController::changeParams(control_system::paramsConfig &msg, int ThrusterSelection){
        int n_map = _controlParams_index[_gain_file];
        double *offset = _controlParams[n_map]->getOffset();
        double **gain = _controlParams[n_map]->getGain();
        offset[ThrusterSelection] = msg.offset;
        gain[ThrusterSelection][0] = msg.Gain_px;
        gain[ThrusterSelection][1] = msg.Gain_py;
        gain[ThrusterSelection][2] = msg.Gain_pz;
        gain[ThrusterSelection][3] = msg.Gain_vx;
        gain[ThrusterSelection][4] = msg.Gain_vy;
        gain[ThrusterSelection][5] = msg.Gain_vz;
        gain[ThrusterSelection][6] = msg.Gain_i_px;
        gain[ThrusterSelection][7] = msg.Gain_i_py;
        gain[ThrusterSelection][8] = msg.Gain_i_pz;
        gain[ThrusterSelection][9] = msg.Gain_ax;
        gain[ThrusterSelection][10] = msg.Gain_ay;
        gain[ThrusterSelection][11] = msg.Gain_az;
        gain[ThrusterSelection][12] = msg.Gain_i_vx;
        gain[ThrusterSelection][13] = msg.Gain_i_vy;
        gain[ThrusterSelection][14] = msg.Gain_i_vz;

        gain[ThrusterSelection][15] = msg.Gain_roll;
        gain[ThrusterSelection][16] = msg.Gain_pitch;
        gain[ThrusterSelection][17] = msg.Gain_yaw;
        gain[ThrusterSelection][18] = msg.Gain_vel_roll;
        gain[ThrusterSelection][19] = msg.Gain_vel_pitch;
        gain[ThrusterSelection][20] = msg.Gain_vel_yaw;
        gain[ThrusterSelection][21] = msg.Gain_i_roll;
        gain[ThrusterSelection][22] = msg.Gain_i_pitch;
        gain[ThrusterSelection][23] = msg.Gain_i_yaw;
        gain[ThrusterSelection][24] = msg.Gain_acc_roll;
        gain[ThrusterSelection][25] = msg.Gain_acc_pitch;
        gain[ThrusterSelection][26] = msg.Gain_acc_yaw;
        gain[ThrusterSelection][27] = msg.Gain_i_vel_roll;
        gain[ThrusterSelection][28] = msg.Gain_i_vel_pitch;
        gain[ThrusterSelection][29] = msg.Gain_i_vel_yaw;

        offset[ThrusterSelection+1] = msg.offset;
        gain[ThrusterSelection+1][0] = msg.Gain_px;
        gain[ThrusterSelection+1][1] = msg.Gain_py;
        gain[ThrusterSelection+1][2] = msg.Gain_pz;
        gain[ThrusterSelection+1][3] = msg.Gain_vx;
        gain[ThrusterSelection+1][4] = msg.Gain_vy;
        gain[ThrusterSelection+1][5] = msg.Gain_vz;
        gain[ThrusterSelection+1][6] = msg.Gain_i_px;
        gain[ThrusterSelection+1][7] = msg.Gain_i_py;
        gain[ThrusterSelection+1][8] = msg.Gain_i_pz;
        gain[ThrusterSelection+1][9] = msg.Gain_ax;
        gain[ThrusterSelection+1][10] = msg.Gain_ay;
        gain[ThrusterSelection+1][11] = msg.Gain_az;
        gain[ThrusterSelection+1][12] = msg.Gain_i_vx;
        gain[ThrusterSelection+1][13] = msg.Gain_i_vy;
        gain[ThrusterSelection+1][14] = msg.Gain_i_vz;

        gain[ThrusterSelection+1][15] = msg.Gain_roll;
        gain[ThrusterSelection+1][16] = msg.Gain_pitch;
        gain[ThrusterSelection+1][17] = msg.Gain_yaw;
        gain[ThrusterSelection+1][18] = msg.Gain_vel_roll;
        gain[ThrusterSelection+1][19] = msg.Gain_vel_pitch;
        gain[ThrusterSelection+1][20] = msg.Gain_vel_yaw;
        gain[ThrusterSelection+1][21] = msg.Gain_i_roll;
        gain[ThrusterSelection+1][22] = msg.Gain_i_pitch;
        gain[ThrusterSelection+1][23] = msg.Gain_i_yaw;
        gain[ThrusterSelection+1][24] = msg.Gain_acc_roll;
        gain[ThrusterSelection+1][25] = msg.Gain_acc_pitch;
        gain[ThrusterSelection+1][26] = msg.Gain_acc_yaw;
        gain[ThrusterSelection+1][27] = msg.Gain_i_vel_roll;
        gain[ThrusterSelection+1][28] = msg.Gain_i_vel_pitch;
        gain[ThrusterSelection+1][29] = msg.Gain_i_vel_yaw;

        //std::string str = "/home/yash/auv_ws/src/kraken_reboot/control_system_stack/control_system/parameters/";
        std::string str = "/home/teamauv/teamauv_ws/src/kraken_reboot/control_system_stack/control_system/parameters/";
        std::fstream fp;
        str = str.append(_gain_file).c_str();
        fp.open(str.append(".cp").c_str(), std::ios::trunc | std::ios::out);
        if(fp.is_open()) _controlParams[n_map]->write(&fp);
        else ROS_INFO("UNABLE TO OPEN FILE %s", _gain_file.c_str());
        std::vector<std::string> filenames;
    }

    bool StateController::checkError(){
        for(int i = 0; i<3; i++){//check goaltype
            //std::cout<<"GoalType"<<GoalType;
            if(GoalType == 0){
                if(_pose_error[i] >= 0.05 || _vel_error[i] >= 0.05)
                    return false;
            }
            else if(GoalType == 1){
                if(_pose_error[i+9] >= 0.5 && _vel_error[i+9] >= 0.5)
                    return false;
            }
        }
        return true;
    }

    void StateController::setThrusters(kraken_msgs::thrusterData6Thruster *thrust){
        int n_map = _controlParams_index[_gain_file];
        double *offset = _controlParams[n_map]->getOffset();
        double **gain = _controlParams[n_map]->getGain();
        //std::cout<<"NOTcalculating error BC";
        //_controlParams[n_map]->write(std::cerr);
<<<<<<< HEAD
        if(GoalType == 0){
            thrust->data[2] = offset[0] + gain[0][0]*_pose_error[0] + gain[0][3]*_pose_error[3] + gain[0][6]*_pose_error[6] - gain[0][15]*_pose_error[11] - gain[0][18]*_pose_error[14] - gain[0][21]*_pose_error[17];
=======
            //std::cout<<"calculating error BC\n";

            thrust->data[2] = offset[2] + gain[2][0]*_pose_error[0] + gain[2][3]*_pose_error[3] + gain[2][6]*_pose_error[6] - gain[2][17]*_pose_error[11] - gain[2][20]*_pose_error[14] - gain[2][23]*_pose_error[17];
>>>>>>> upstream/testing-controls

            thrust->data[3] = offset[3] + gain[3][0]*_pose_error[0] + gain[3][3]*_pose_error[3] + gain[3][6]*_pose_error[6] + gain[3][17]*_pose_error[11] + gain[3][20]*_pose_error[14] + gain[3][23]*_pose_error[17];

            thrust->data[4] = 0;//offset[2] + gain[2][0]*_pose_error[0] + gain[2][3]*_pose_error[3] + gain[2][6]*_pose_error[6] + gain[2][15]*_pose_error[9] + gain[2][19]*_pose_error[12] + gain[2][21]*_pose_error[15];
            thrust->data[5] = 0;//offset[3] + gain[3][0]*_pose_error[0] + gain[3][3]*_pose_error[3] + gain[3][6]*_pose_error[6] + gain[3][15]*_pose_error[9] + gain[3][19]*_pose_error[12] + gain[3][21]*_pose_error[15];
<<<<<<< HEAD
            thrust->data[0] = offset[4] + gain[4][2]*_pose_error[2] + gain[4][5]*_pose_error[5] + gain[4][8]*_pose_error[8] + gain[4][17]*_pose_error[10] + gain[4][20]*_pose_error[13] + gain[4][23]*_pose_error[16];

            thrust->data[1] = offset[5] + gain[5][2]*_pose_error[2] + gain[5][5]*_pose_error[5] + gain[5][8]*_pose_error[8] - gain[5][17]*_pose_error[10] - gain[5][20]*_pose_error[13] - gain[5][23]*_pose_error[16];
        }
=======
            thrust->data[0] = offset[0] + gain[0][2]*_pose_error[2] + gain[0][5]*_pose_error[5] + gain[0][8]*_pose_error[8] + gain[0][16]*_pose_error[10] + gain[0][19]*_pose_error[13] + gain[0][22]*_pose_error[16];
            thrust->data[1] = offset[1] + gain[1][2]*_pose_error[2] + gain[1][5]*_pose_error[5] + gain[1][8]*_pose_error[8] - gain[1][16]*_pose_error[10] - gain[5][19]*_pose_error[13] - gain[1][22]*_pose_error[16];
            // for(int i = 0; i<30; i++){
            //     std::cout<<i<<"---"<<gain[1][i]<<"\n";
            //  }
            //std::cout<<thrust->data[0]<<"\n"<<thrust->data[1]<<"\n"<<thrust->data[2]<<"\n"<<thrust->data[3]<<"\n";
>>>>>>> upstream/testing-controls
        /*else{
            thrust->data[0] = offset[0] + gain[0][0]*_vel_error[0] + gain[0][3]*_vel_error[3] + gain[0][6]*_vel_error[6] + gain[0][15]*_vel_error[11] + gain[0][18]*_vel_error[14] + gain[0][21]*_vel_error[17];

            thrust->data[1] = offset[1] + gain[1][0]*_vel_error[0] + gain[1][3]*_vel_error[3] + gain[1][6]*_vel_error[6] + gain[1][15]*_vel_error[11] + gain[1][18]*_vel_error[14] + gain[1][21]*_vel_error[17];

            thrust->data[2] = 0;//offset[2] + gain[2][0]*_vel_error[0] + gain[2][3]*_vel_error[3] + gain[2][6]*_vel_error[6] + gain[2][15]*_vel_error[9] + gain[2][19]*_vel_error[12] + gain[2][21]*_vel_error[15];
            thrust->data[3] = 0;//offset[3] + gain[3][0]*_vel_error[0] + gain[3][3]*_vel_error[3] + gain[3][6]*_vel_error[6] + gain[3][15]*_vel_error[9] + gain[3][19]*_vel_error[12] + gain[3][21]*_vel_error[15];
            thrust->data[4] = offset[4] + gain[4][2]*_vel_error[2] + gain[4][5]*_vel_error[5] + gain[4][8]*_vel_error[8] + gain[4][17]*_vel_error[10] + gain[4][20]*_vel_error[13] + gain[4][23]*_vel_error[16];
            thrust->data[5] = offset[5] + gain[5][2]*_vel_error[2] + gain[5][5]*_vel_error[5] + gain[5][8]*_vel_error[8] + gain[5][17]*_vel_error[10] + gain[5][20]*_vel_error[13] + gain[5][23]*_vel_error[16];
        }*/

    }
}