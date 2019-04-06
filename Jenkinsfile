pipeline {
    agent { dockerfile true }
    environment {
        BINTRAY_CRED = credentials('Bintray')
        BINTRAY_USER = "${env.BINTRAY_CRED_USR}"
        BINTRAY_API_KEY = "${env.BINTRAY_CRED_PSW}"
    }
    
    stages {
        stage('Checkout') {
            steps {
                checkout scm
                
                script {
                    if (isUnix()) {
                        sh 'chmod +x ./gradlew'
                    }
                }
            }
        }
        stage('Build') {
            steps {
                sh './gradlew build'
            }
        }
        stage('Lint') {
            steps {
                androidLint canComputeNew: false, canRunOnFailed: true, defaultEncoding: '', healthy: '', pattern: '', unHealthy: ''
            }
        }
        stage('Deploy') {
            when {
                branch 'master'
            }
            steps {
                sh './gradlew bintrayUpload'
            }
        }
    }
    post {
        success {
            archiveArtifacts '**/*.aar'
        }
    }
}